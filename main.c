#define SAMPLE_RATE  44100
#define NUM_CHANNELS 2
/*
 * 1. Record audio to a buffer.
 * 2. Save buffer to disk.
 * 3. Read buffer from disk.
 * 4. Play audio.
 */

struct userdata {
    int len, cap;
    float *data;
};

static int icallback(
  const void *input,
  void *output,
  unsigned long nframes,
  const PaStreamCallbackTimeInfo* timeinfo,
  PaStreamCallbackFlags flags,
  void *userdata)
{
	struct userdata *ud = userdata;
	int status;

	(void) output;
	(void) timeinfo;
	(void) flags;

	unsigned rem = ud->cap - ud->len;
	if (rem < nframes) {
		nframes = rem;
		status = paComplete;
	} else {
		status = paContinue;
	}

	float *wp = &ud->data[ud->len];
	const float *rp = input;
	for (unsigned i=0; rp && i<nframes; ++i, ++ud->len)
		wp[i] = rp[i];
	return status;
}

static int ocallback(
  const void *input,
  void *output,
  unsigned long nframes,
  const PaStreamCallbackTimeInfo* timeinfo,
  PaStreamCallbackFlags flags,
  void *userdata)
{
    paTestData *data = (paTestData*)userData;
    SAMPLE *rptr = &data->recordedSamples[data->frameIndex * NUM_CHANNELS];
    SAMPLE *wptr = (SAMPLE*)outputBuffer;
    unsigned int i;
    int finished;
    unsigned int framesLeft = data->maxFrameIndex - data->frameIndex;

    (void) inputBuffer; /* Prevent unused variable warnings. */
    (void) timeInfo;
    (void) statusFlags;
    (void) userData;

    if( framesLeft < framesPerBuffer )
    {
        /* final buffer... */
        for( i=0; i<framesLeft; i++ )
        {
            *wptr++ = *rptr++;  /* left */
            if( NUM_CHANNELS == 2 ) *wptr++ = *rptr++;  /* right */
        }
        for( ; i<framesPerBuffer; i++ )
        {
            *wptr++ = 0;  /* left */
            if( NUM_CHANNELS == 2 ) *wptr++ = 0;  /* right */
        }
        data->frameIndex += framesLeft;
        finished = paComplete;
    }
    else
    {
        for( i=0; i<framesPerBuffer; i++ )
        {
            *wptr++ = *rptr++;  /* left */
            if( NUM_CHANNELS == 2 ) *wptr++ = *rptr++;  /* right */
        }
        data->frameIndex += framesPerBuffer;
        finished = paContinue;
    }
    return finished;
}

void initialize(void) {
	PaError err = Pa_Initialize();
	if (err != paNoError) {
		fprintf(stderr, "PortAudio initialization error: %s\n",
		  Pa_GetErrorText(err));
		exit(EXIT_FAILURE);
	}
}

void terminate() {
	PaError err = Pa_Terminate();
	if (err != paNoError) {
		fprintf(stderr, "PortAudio termination error: %s\n",
		  Pa_GetErrorText(err));
		exit(EXIT_FAILURE);
	}
}

void start(PaStream *stream) {
	PaError err = Pa_StartStream(stream);
	if (err != paNoError) {
		fprintf(stderr, "PortAudio start stream error: %s\n",
		  Pa_GetErrorText(err));
		exit(EXIT_FAILURE);
	}
}

void stop(PaStream *stream) {
	PaError err = Pa_StopStream(stream);
	if (err != paNoError) {
		fprintf(stderr, "PortAudio start stream error: %s\n",
		  Pa_GetErrorText(err));
		exit(EXIT_FAILURE);
	}
}

void idle(int seconds) {
	Pa_Sleep(seconds*1000);
}

int main() {
	PaStream *istream, *ostream;
	PaError err;

	initialize();

	struct userdata ud = {0};
	err = Pa_OpenDefaultStream(
	  &istream, 2, 0, paFloat32, SAMPLE_RATE, paFramesPerBufferUnspecified,
	  icallback, &ud);
	if (err != paNoError) {
		fprintf(stderr, "PortAudio open default stream error: %s\n",
		  Pa_GetErrorText(err));
		exit(EXIT_FAILURE);
	}

	start(istream);
	stop(istream);

	err = Pa_CloseStream(istream);
	if (err != paNoError) {
		fprintf(stderr, "PortAudio close stream error: %s\n",
		  Pa_GetErrorText(err));
		exit(EXIT_FAILURE);
	}

	terminate();
}
