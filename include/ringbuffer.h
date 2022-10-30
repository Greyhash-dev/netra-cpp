#include <vector>

class Ringbuffer
{
	private:
        int cur = 0;

	public:
        Ringbuffer(int size);
        int arr_size;
        ~Ringbuffer();
        std::vector<int> buffer;
        void add(int x);
};
