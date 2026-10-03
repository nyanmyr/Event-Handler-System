#include <iostream>
#include <functional>
#include <vector>
#include <unordered_map>
#include <typeindex>
#include <typeinfo>
#include <any>

namespace Event
{
    struct MouseClick
    {
        int x;
        int y;
    };
}

// stores events and listensers (functions)
class Dispatcher
{
    private:
    std::unordered_map<std::type_index, std::any> events;

    template<typename T>
    std::vector<std::function<void(T)>>* getEventArray()
	{
		std::type_index typeName = std::type_index(typeid(T));

		auto it = events.find(typeName);
		if (it == events.end())
		{
			throw std::runtime_error("Event not registered.");
		}

        return std::any_cast<std::vector<std::function<void(T)>>>(&(it->second));
	}

    public:
    template<typename T>
    void registerEvent()
    {
        std::type_index typeName = std::type_index(typeid(T));

        if (events.find(typeName) != events.end())
		{
			throw std::runtime_error("Event already registered.");
		}

        events.insert({ typeName, std::vector<std::function<void(T)>>{} });
    }

    template<typename T>
	void listen(std::function<void(T)> x)
	{
		getEventArray<T>()->push_back(x); 
	}

    template<typename T>
	void call(T x)
	{
        for (std::function<void(T)> func : *getEventArray<T>())
        {
            func(x);
        }
	}
};

namespace Listenser
{
    void printPos(Event::MouseClick event)
    {
        std::cout << "x: " << event.x << " y: " << event.y << "\n";
    }

    const int BOUNDS_X = 10;
    const int BOUNDS_Y = 10;

    void checkBoundsX(Event::MouseClick event)
    {
        if (event.x > BOUNDS_X) std::cout << "x boundary crossed!" << "\n";
    }

    void checkBoundsY(Event::MouseClick event)
    {
        if (event.y > BOUNDS_Y) std::cout << "y boundary crossed!" << "\n";
    }
}

int main() 
{
    Dispatcher dis{};

    dis.registerEvent<Event::MouseClick>();

    // listener creation
    std::function<void(Event::MouseClick)> listener1 = Listenser::printPos;
    std::function<void(Event::MouseClick)> listener2 = Listenser::checkBoundsX;
    std::function<void(Event::MouseClick)> listener3 = Listenser::checkBoundsY;

    // listener registration
    dis.listen<Event::MouseClick>(listener1);
    dis.listen<Event::MouseClick>(listener2);
    dis.listen<Event::MouseClick>(listener3);

    dis.call<Event::MouseClick>
    (
        Event::MouseClick
        { 10, 20 }
    );

    return 0;
}