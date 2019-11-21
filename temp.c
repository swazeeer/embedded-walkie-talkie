/*
	Filename: USB_headphones.c
	Description: code for proof of concept to use USB headphones for audio
	Author: Christopher Bolognese
		Based on ALSA guide: http://equalarea.com/paul/alsa-audio.html
	Date: Oct. 25, 2015
*/

#include <stdio.h>
#include <stdlib.h>
#include <alsa/asoundlib.h>

#define AUDIO_DEVICE "plughw:1,0"
//#define AUDIO_DEVICE "default"
#define SAMPLE_RATE   44100
#define NUM_CHANNELS  2
#define BUF_SIZE	128
//arecord -d 6 --device=B -r 44100 -f S16_LE -c 2 testRecording.wav
static snd_pcm_t *input_handle;
static snd_pcm_t *output_handle;
//multiply by number of channels since a frame is one sample per channel
static short buf[BUF_SIZE * NUM_CHANNELS];

//local headers
static void initInput(void);
static void initOutput(void);
static void playAudio(void);
 //ext4write mmc 1:1 0x82000000 /etc/systemd/system/myservice.service 0
//arecord -d 7 -D default:AK5370 -r 44100 -f S16_LE -c 2 testRecording.wav
/*
‘-vn’   Disable video. 

‘listen’

    Act as a server, listening for an incoming connection.

‘-nodisp’

    Disable graphical display. 

	‘-noautoexit’

    Do not exit after playback is finished. 
‘-exitonkeydown’

    Exit if any key is pressed. 
‘-exitonmousedown’

    Exit if any mouse button is pressed. 
‘-noautorotate’

    Disable automatically rotating video based on file metadata. 

*/
//avconv  -f alsa -ac 1 -i hw:AK5370 -acodec mp2 -b 64k  -f rtp rtp://192.168.1.90:12346
//sudo avplay -nodisp -i rtp://192.168.1.80:12346
//>/dev/null 2>&1
/*
you have to start avconv first then start avplay
is there a way to get avplay to keep listening 
n then we can toggle avconv as needed 
*/
static void initInput(void){
	int err;
	snd_pcm_hw_params_t *hw_params;

	if ((err = snd_pcm_open (&input_handle, AUDIO_DEVICE, SND_PCM_STREAM_CAPTURE, 0)) < 0) {
		fprintf (stderr, "cannot open audio device %s (%s)\n",
				AUDIO_DEVICE,
			 snd_strerror (err));
		exit(EXIT_FAILURE);
	}

	if ((err = snd_pcm_hw_params_malloc (&hw_params)) < 0) {
		fprintf (stderr, "cannot allocate hardware parameter structure (%s)\n",
			 snd_strerror (err));
		exit(EXIT_FAILURE);
	}

	if ((err = snd_pcm_hw_params_any (input_handle, hw_params)) < 0) {
		fprintf (stderr, "cannot initialize hardware parameter structure (%s)\n",
			 snd_strerror (err));
		exit(EXIT_FAILURE);
	}

	if ((err = snd_pcm_hw_params_set_access (input_handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0) {
		fprintf (stderr, "cannot set access type (%s)\n",
			 snd_strerror (err));
		exit(EXIT_FAILURE);
	}

	if ((err = snd_pcm_hw_params_set_format (input_handle, hw_params, SND_PCM_FORMAT_S16_LE)) < 0) {
		fprintf (stderr, "cannot set sample format (%s)\n",
			 snd_strerror (err));
		exit(EXIT_FAILURE);
	}
	unsigned int rate = SAMPLE_RATE;
	if ((err = snd_pcm_hw_params_set_rate_near (input_handle, hw_params, &rate, 0)) < 0) {
		fprintf (stderr, "cannot set sample rate (%s)\n",
			 snd_strerror (err));
		exit(EXIT_FAILURE);
	}

	if ((err = snd_pcm_hw_params_set_channels (input_handle, hw_params, NUM_CHANNELS)) < 0) {
		fprintf (stderr, "cannot set channel count (%s)\n",
			 snd_strerror (err));
		exit(EXIT_FAILURE);
	}

	if ((err = snd_pcm_hw_params (input_handle, hw_params)) < 0) {
		fprintf (stderr, "cannot set parameters (%s)\n",
			 snd_strerror (err));
		exit(EXIT_FAILURE);
	}

	snd_pcm_hw_params_free (hw_params);

	if ((err = snd_pcm_prepare (input_handle)) < 0) {
		fprintf (stderr, "cannot prepare audio interface for use (%s)\n",
			 snd_strerror (err));
		exit(EXIT_FAILURE);
	}
}
static void initOutput(void){
	// Open the PCM output
	int err = snd_pcm_open(&output_handle, AUDIO_DEVICE, SND_PCM_STREAM_PLAYBACK, 0);
	if (err < 0) {
		printf("Play-back open error: %s\n", snd_strerror(err));
		exit(EXIT_FAILURE);
	}

	// Configure parameters of PCM output
	err = snd_pcm_set_params(output_handle,
			SND_PCM_FORMAT_S16_LE,
			SND_PCM_ACCESS_RW_INTERLEAVED,
			NUM_CHANNELS,
			SAMPLE_RATE,
			1,			// Allow software resampling
			50000);		// 0.05 seconds per buffer
	if (err < 0) {
		printf("Play-back configuration error: %s\n", snd_strerror(err));
		exit(EXIT_FAILURE);
	}
}

static void playAudio(void){
	snd_pcm_sframes_t frames = snd_pcm_writei(output_handle, buf, BUF_SIZE);
	// Check for errors
	if (frames < 0)
		frames = snd_pcm_recover(output_handle, frames, 0);
	if (frames < 0) {
		fprintf(stderr, "ERROR: Failed writing audio with snd_pcm_writei(): %li\n", frames);
		exit(EXIT_FAILURE);
	}
	if (frames > 0 && frames < BUF_SIZE)
		printf("Short write (expected %d, wrote %li)\n", BUF_SIZE, frames);

}

int main (int argc, char *argv[]) {
	int err;
	initInput();
	initOutput();

	for (int i = 0; i < 7000; ++i) {
        printf("%i \n " , i);
		if ((err = snd_pcm_readi (input_handle, buf, BUF_SIZE)) != BUF_SIZE) {
			fprintf (stderr, "read from audio interface failed (%s)\n",
				 snd_strerror (err));
			exit(EXIT_FAILURE);
		}
		playAudio();
	}

	snd_pcm_close (input_handle);
	snd_pcm_close (output_handle);
	return EXIT_SUCCESS;
}
