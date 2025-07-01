#include <iostream>
#include <thread>
#include <vector>
#include <random>
#include <chrono>
#include <cmath>
#include <mutex>
#include <string>

using namespace std;

const int w = 800;
const int h = 600;
const int V = 5;
const int N = 10; // змінювати напрямок кожні N ітерацій
const int delayMs = 100;

int iterations = 50; // змінювана кількість ітерацій

std::mutex print_mutex;
std::mt19937 rng(std::random_device{}());

struct Point {
    float x, y;
};

// Базовий клас кролика
class Rabbit {
protected:
    Point position;
    string name;
public:
    Rabbit(Point pos, const string& typeName) : position(pos), name(typeName) {}
    virtual void move() = 0;
};

// Звичайний кролик (хаотичний)
class NormalRabbit : public Rabbit {
public:
    NormalRabbit(Point pos, int id) : Rabbit(pos, "Zvychainyi krolyk #" + to_string(id)) {}

    void move() override {
        float angle = rand() % 360;
        float dx = V * cos(angle);
        float dy = V * sin(angle);

        for (int i = 0; i < iterations; ++i) {
            if (i % N == 0) {
                angle = rand() % 360;
                dx = V * cos(angle);
                dy = V * sin(angle);
            }

            position.x += dx;
            position.y += dy;

            // Межі області
            if (position.x < 0) position.x = 0;
            if (position.y < 0) position.y = 0;
            if (position.x > w) position.x = w;
            if (position.y > h) position.y = h;

            {
                lock_guard<mutex> lock(print_mutex);
                cout << name << " (khaotychno) na pozytsii: (" << position.x << ", " << position.y << ")" << endl;
            }

            this_thread::sleep_for(chrono::milliseconds(delayMs));
        }
    }
};

// Альбінос кролик (по осі X)
class AlbinoRabbit : public Rabbit {
public:
    AlbinoRabbit(Point pos, int id) : Rabbit(pos, "Krolyk-albinos #" + to_string(id)) {}

    void move() override {
        for (int i = 0; i < iterations; ++i) {
            position.x += V;
            if (position.x > w) position.x = 0;

            {
                lock_guard<mutex> lock(print_mutex);
                cout << name << " (po X) na pozytsii: (" << position.x << ", " << position.y << ")" << endl;
            }

            this_thread::sleep_for(chrono::milliseconds(delayMs));
        }
    }
};

int main() {
    int numNormal = 2;
    int numAlbino = 2;

    cout << "Vvedit kilkist iteratsii symuliatsii: ";
    cin >> iterations;

    vector<thread> threads;

    // Звичайні кролики
    for (int i = 0; i < numNormal; ++i) {
        Point start = { static_cast<float>(rand() % w), static_cast<float>(rand() % h) };
        NormalRabbit* r = new NormalRabbit(start, i + 1);
        threads.emplace_back(&NormalRabbit::move, r);
    }

    // Альбіноси
    for (int i = 0; i < numAlbino; ++i) {
        Point start = { static_cast<float>(rand() % w), static_cast<float>(rand() % h) };
        AlbinoRabbit* r = new AlbinoRabbit(start, i + 1);
        threads.emplace_back(&AlbinoRabbit::move, r);
    }

    for (auto& t : threads) {
        t.join();
    }

    cout << "\nSymuliatsiia zavershena!" << endl;
    return 0;
}
