#include <bits/stdc++.h>

std::atomic<int> sum;
std::mutex glob_mtx;
std::condition_variable var;

template <typename T>
class Queue{
    public:
        void push(const T &obj) {
            std::unique_lock<std::mutex> lock(mtx);
            que.push(obj);
            
            cv.notify_one();
        }

        T pop() {
            std::unique_lock<std::mutex> lock(mtx);
            cv.wait(lock, [this] () { return !que.empty(); });
            T obj = que.front();
            que.pop();
            
            return obj;
        }

    private:
        std::queue<T> que;
        std::mutex mtx;
        std::condition_variable cv;

        bool empty() {
            std::unique_lock<std::mutex> lock(mtx);
            return que.empty();
        }
};


class Image {
    public:
        std::vector<int> data;
        int w, h, ch;

        Image(int _w, int _h, int _ch = 3) : w(_w), h(_h), ch(_ch){
            data.resize(w*h*3);
        }
};

Queue<Image> buffer;

void random_sleep(int mod) {
    int lol = rand() % mod + 10;
    std::this_thread::sleep_for(std::chrono::milliseconds(lol));
}

void Producer(int id, int w, int h) {
    for(int k = 0; k < 30; k++) {
        Image img(w, h);
        const int sz = img.data.size();

        for(int i = 0; i < sz; i++) {
            img.data[i] = rand() % 256;
        }

        std::cout << "Producer " << id << " : Generated image " << k << std::endl;

        random_sleep(50);

        buffer.push(img);
    
        random_sleep(1);
    }
}

void invertColors(Image &img) {
    for (size_t i = 0; i < img.data.size(); ++i) {
        img.data[i] = 255 - img.data[i];  // Инвертируем каждый канал
    }
    random_sleep(20);
}

void Consumer(int id) {
    while(true) {
        Image img = buffer.pop();
        invertColors(img);
        std::cout << "Consumer " << id << " processed image {" << img.w << ", " << img.h << "}" << std::endl; 
        sum++;
    }
}



int main() {
    std::srand(std::time(0));

    //int img_cnt = 100;
    const int width = 640;
    const int height = 480;
    const int numConsumers = 4;
    const int numProducers = 3;
    std::vector<std::thread> prodThreads;
    std::vector<std::thread> consThreads;
    
    for(int i = 0; i < numProducers; i++) {
        prodThreads.emplace_back(Producer, i, width, height);
    }

    for(int i = 0; i < numConsumers; i++) {
        consThreads.emplace_back(Consumer, i);
    }

    for(auto &i: prodThreads) {
        i.join();
    }

    for(auto &i: consThreads) {
        i.join();
    }


    if(sum == 90)
        var.notify_all();
    {
        std::unique_lock<std::mutex> lock(glob_mtx);
        var.wait(lock, []() { return sum == 90; }); 
    } 


    return 0;
}