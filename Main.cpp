import RayTracer;

int main()
{
	// Image
	constexpr const float ASPECT_RATIO = 3.0f / 2.0f;
	constexpr const size_t IMAGE_WIDTH = 1200u;
	constexpr const size_t IMAGE_HEIGHT = static_cast<size_t>(IMAGE_WIDTH / ASPECT_RATIO);
	constexpr const size_t NUM_CHANNELS = 3u;

	// Render
	RayTracer rayTracer(IMAGE_WIDTH, IMAGE_HEIGHT, NUM_CHANNELS);

	rayTracer.Initialize();
	rayTracer.Render();

	return 0;
}