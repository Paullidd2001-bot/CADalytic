#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

#include "EventBus.h"
#include "EventTypes.h"

namespace {

void require(bool condition, const std::string& message)
{
    if (!condition) {
        std::cerr << "FAIL: " << message << '\n';
        std::exit(EXIT_FAILURE);
    }
}

void testSubscribeAndReceiveEvent()
{
    cadalytic::EventBus bus;
    std::vector<cadalytic::Event> received;
    bus.subscribe([&](const cadalytic::Event& event) {
        received.push_back(event);
    });

    cadalytic::Event event{cadalytic::EventType::DocumentChanged, 1, 2, "doc"};
    bus.publish(event);

    require(received.size() == 1, "subscribed handler should receive one event");
    require(received[0].type == event.type, "event type should be delivered");
    require(received[0].sourceId == 1 && received[0].targetId == 2,
            "event ids should be delivered");
    require(received[0].detail == "doc", "event detail should be delivered");
}

void testMultipleSubscribersAllReceive()
{
    cadalytic::EventBus bus;
    int countA = 0;
    int countB = 0;
    bus.subscribe([&](const cadalytic::Event&) { ++countA; });
    bus.subscribe([&](const cadalytic::Event&) { ++countB; });

    bus.publish(cadalytic::Event{cadalytic::EventType::FeatureAdded, 0, 0, ""});

    require(countA == 1 && countB == 1, "every subscriber should receive the event");
}

void testUnsubscribeStopsDelivery()
{
    cadalytic::EventBus bus;
    int kept = 0;
    int dropped = 0;
    const auto droppedId = bus.subscribe([&](const cadalytic::Event&) { ++dropped; });
    bus.subscribe([&](const cadalytic::Event&) { ++kept; });

    require(bus.unsubscribe(droppedId), "unsubscribe should report success");
    require(!bus.unsubscribe(999999), "unsubscribing an unknown id should fail");

    bus.publish(cadalytic::Event{cadalytic::EventType::FeatureRemoved, 0, 0, ""});

    require(kept == 1, "remaining subscriber should still receive events");
    require(dropped == 0, "unsubscribed handler should not be called");
    require(bus.subscriberCount() == 1, "subscriber count should drop by one");
}

void testClearRemovesAllSubscribers()
{
    cadalytic::EventBus bus;
    int count = 0;
    bus.subscribe([&](const cadalytic::Event&) { ++count; });
    bus.subscribe([&](const cadalytic::Event&) { ++count; });

    require(bus.subscriberCount() == 2, "two subscribers registered");
    bus.clear();
    require(bus.subscriberCount() == 0, "clear should remove all subscribers");

    bus.publish(cadalytic::Event{cadalytic::EventType::SelectionChanged, 0, 0, ""});
    require(count == 0, "no subscriber should fire after clear");
}

void testReentrantSubscribeIsSafe()
{
    // Publishing must iterate over a snapshot, so a handler that subscribes a
    // new handler during dispatch must not receive the current event.
    cadalytic::EventBus bus;
    int active = 0;
    int late = 0;
    bus.subscribe([&](const cadalytic::Event&) {
        ++active;
        bus.subscribe([&](const cadalytic::Event&) { ++late; });
    });

    bus.publish(cadalytic::Event{cadalytic::EventType::CommandExecuted, 0, 0, ""});

    require(active == 1, "snapshot dispatch should deliver to the original handler");
    require(late == 0, "handler subscribed during publish should not see the current event");
    require(bus.subscriberCount() == 2, "late handler should be registered for the next event");

    // The late handler DOES see subsequent events.
    bus.publish(cadalytic::Event{cadalytic::EventType::UndoPerformed, 0, 0, ""});
    require(late == 1, "late handler should receive later events");
}

} // namespace

int main()
{
    testSubscribeAndReceiveEvent();
    testMultipleSubscribersAllReceive();
    testUnsubscribeStopsDelivery();
    testClearRemovesAllSubscribers();
    testReentrantSubscribeIsSafe();
    std::cout << "All event tests passed.\n";
    return EXIT_SUCCESS;
}
