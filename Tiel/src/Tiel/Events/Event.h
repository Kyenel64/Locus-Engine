// --- Event ------------------------------------------------------------------
// Contains abstract class for events and the event dispatcher.

#pragma once

#include "tpch.h"
#include "Tiel/Core.h"

namespace Tiel
{
	enum class EventType // Enum classes dont convert to integers
	{
		None = 0,
		// Window events
		WindowClose, WindowResize, WindowFocus, WindowLostFocus, WindowMoved,
		// Application event types
		AppTick, AppUpdate, AppRender,
		// Key event types
		KeyPressed, KeyReleased, KeyTyped,
		// Mouse event types
		MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled
	};

	enum EventCategory
	{
		None = 0,
		EventCategoryApplication = BIT(0),
		EventCategoryInput = BIT(1),
		EventCategoryKeyboard = BIT(2),
		EventCategoryMouse = BIT(3),
		EventCategoryMouseButton = BIT(4)
	};

#define EVENT_CLASS_TYPE(type) static EventType GetStaticType() { return EventType::type; }\
								virtual EventType GetEventType() const override { return GetStaticType(); }\
								virtual const char* GetName() const override { return #type; }

#define EVENT_CLASS_CATEGORY(category) virtual int GetCategoryFlags() const override { return category; }

	// Parent class for events
	class TIEL_API Event
	{
		friend class EventDispatcher; // Friend classes can access private and protected.
	public:
		virtual EventType GetEventType() const = 0; // Virtual functions are meant to be overridden.
		virtual const char* GetName() const = 0;
		virtual int GetCategoryFlags() const = 0;
		virtual std::string ToString() const { return GetName(); }

		// Inline fills function within code. Doesn't search for function elsewhere.
		inline bool IsInCategory(EventCategory category)
		{
			return GetCategoryFlags() & category;
		}

		bool m_Handled = false;
	};

	// Dispatches event based on event type
	class EventDispatcher
	{
		template<typename T>
		using EventFn = std::function<bool(T&)>;
	public:
		EventDispatcher(Event& event) : m_Event(event) {}

		template<typename T>
		bool Dispatch(EventFn<T> func) // passes function as parameter
		{
			// If dispatch event matches event
			if (m_Event.GetEventType() == T::GetStaticType())
			{
				m_Event.m_Handled = func(*(T*)&m_Event);
				return true;
			}
			return false;
		}
	private:
		Event& m_Event;
	};

	// Output override for event class
	inline std::ostream& operator<<(std::ostream& os, const Event& e)
	{
		return os << e.ToString();
	}
}