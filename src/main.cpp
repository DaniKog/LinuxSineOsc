#include <alsa/asoundlib.h>
#include <math.h>
#define BUFFER_LEN 48000
#define SAMPLE_RATE 48000
#define DEFAULT_FREQUENCY 440.f // Hz
#define DEFAULT_DURATION 3.f // Sec
#define DEFAULT_VOLUME 1.f // Amp

static char* device = "default";                       //soundcard
snd_output_t* output = NULL;
float buffer[BUFFER_LEN];


int main(int argc, char** argv)
{
    int err;
    int i;
    float frequency = DEFAULT_FREQUENCY;                       //frequency in Hz
    float duration = DEFAULT_DURATION;                         //Time to play in seconds  
    float volume = DEFAULT_VOLUME;                             //volume to play in [0..1]

	if (argv[1])
	{
		frequency = atof(argv[1]);
		if (frequency > 50000 || frequency < 1 || !frequency)
		{
			printf("%d Hz is out of range, defaulting to 440Hz \n", frequency);
			frequency = DEFAULT_FREQUENCY;
		}
	}

	if (argv[2])
	{
		duration = atof(argv[2]);
	}

	if (argv[3])
	{
		volume = atof(argv[3]);
		if (volume > 1 || frequency < 0)
		{
			printf("Volume Value %d is out of range, defaulting to Default Volume \n", volume);
			volume = DEFAULT_VOLUME;
		}
	}
  
    snd_pcm_t* handle;
    snd_pcm_sframes_t frames;
    // ERROR HANDLING
    if ((err = snd_pcm_open(&handle, device, SND_PCM_STREAM_PLAYBACK, 0)) < 0) {
        printf("Playback open error: %s\n", snd_strerror(err));
        exit(EXIT_FAILURE);
    }

    if ((err = snd_pcm_set_params(handle,
        SND_PCM_FORMAT_FLOAT,
        SND_PCM_ACCESS_RW_INTERLEAVED,
        1,
        SAMPLE_RATE,
        1,
        500000)) < 0) {
        printf("Playback open error: %s\n", snd_strerror(err));
        exit(EXIT_FAILURE);
    }

    // SINE WAVE
    printf("Sine tone at %.2fHz for %.2f seconds\n", frequency, duration);

    for (i = 0; i < BUFFER_LEN; i++)
    {
        float sampleValue = volume * (float)sin(2 * M_PI * frequency / SAMPLE_RATE * i);  // sine wave value generation
        buffer[i] = sampleValue;

        //Safeguard to not explode speakers
        if (sampleValue > 1.0f || sampleValue < -1.0f)
        {
            printf("Sample Value out of bounds, please check Sine formula\n");
            return 0;
        }
    }

    for (i = 0; i < duration * SAMPLE_RATE / BUFFER_LEN; i++) { // 10 seconds
        frames = snd_pcm_writei(handle, buffer, BUFFER_LEN);

        if (frames < 0)
            frames = snd_pcm_recover(handle, frames, 0);
        if (frames < 0) {
            printf("snd_pcm_writei failed: %s\n", snd_strerror(frames));
            break;
        }
        if (frames > 0 && frames < BUFFER_LEN)
            printf("Short write (expected %li, wrote %li)\n", BUFFER_LEN, frames);
    }

    snd_pcm_close(handle);
    return 0;
}
