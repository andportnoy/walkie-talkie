void devprint(const PaDeviceInfo *dev) {
	printf("%s:", dev->name);
	printf(" srate=%f", dev->defaultSampleRate);
	printf(" input=(%d)", dev->maxInputChannels);
	printf("[%.2fms, %.2fms]",
	  1000*dev->defaultLowInputLatency, 1000*dev->defaultHighInputLatency);
	printf(" output=(%d)", dev->maxOutputChannels);
	printf("[%.2fms, %.2fms]",
	  1000*dev->defaultLowOutputLatency,1000*dev->defaultHighOutputLatency);
	printf("\n");
}

int main() {
	PaError err;
	(void)err;

	/* initialize */
	pacheck(Pa_Initialize(), "initialize");

	for (int i=0, n=Pa_GetDeviceCount(); i<n; ++i)
		devprint(Pa_GetDeviceInfo(i));

	puts("default input:");
	devprint(Pa_GetDeviceInfo(Pa_GetDefaultInputDevice()));

	puts("default output:");
	devprint(Pa_GetDeviceInfo(Pa_GetDefaultOutputDevice()));

	/* open stream */
	/* start stream */
	/* stop stream */
	/* close stream */

	/* terminate */
	pacheck(Pa_Terminate(), "terminate")
}
