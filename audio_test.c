int main() {
	audio_initialize();

	for (;;)
		audio_write(audio_read());

	audio_terminate();
}
