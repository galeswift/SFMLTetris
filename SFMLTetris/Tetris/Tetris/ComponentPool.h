#pragma once

#include "Component.h"

// Capacity of every component pool created by DEFINE_COMPONENT
#define MAX_COMPONENTS 32

// Type-erased view of a pool so the registry and GameManager can address
// pools by ComponentType without knowing the concrete component class.
class ComponentPoolBase
{
public:
	virtual ~ComponentPoolBase() {}
	virtual Component* AllocBase(Tetris* owner) = 0;
	virtual void Free(Component* component) = 0;
	virtual Component* GetBase(int idx) = 0;
	virtual int Capacity() const = 0;
};

// Dead simple fixed-size component pool. Components live in a flat array;
// Alloc hands out the first free slot, Free returns it.
template <typename T, int CAPACITY>
class ComponentPool : public ComponentPoolBase
{
public:
	ComponentPool()
	{
		for (int i = 0; i < CAPACITY; i++)
		{
			m_inUse[i] = false;
		}
	}

	T* Alloc(Tetris* owner)
	{
		for (int i = 0; i < CAPACITY; i++)
		{
			if (!m_inUse[i])
			{
				m_inUse[i] = true;
				m_items[i].m_owner = owner;
				m_items[i].Reset();
				return &m_items[i];
			}
		}
		return NULL;
	}

	virtual Component* AllocBase(Tetris* owner)
	{
		return Alloc(owner);
	}

	virtual void Free(Component* component)
	{
		for (int i = 0; i < CAPACITY; i++)
		{
			if (&m_items[i] == component)
			{
				m_inUse[i] = false;
				m_items[i].m_owner = NULL;
				return;
			}
		}
	}

	virtual Component* GetBase(int idx)
	{
		return m_inUse[idx] ? &m_items[idx] : NULL;
	}

	virtual int Capacity() const { return CAPACITY; }

private:
	T m_items[CAPACITY];
	bool m_inUse[CAPACITY];
};

// One pool factory per ComponentType, filled in during static initialization by
// the registrars that DEFINE_COMPONENT drops in each component's .cpp.
// Each GameManager calls CreatePool to build its own private set of pools,
// so components never leak between managers.
typedef ComponentPoolBase* (*ComponentPoolFactory)();

class ComponentRegistry
{
public:
	static void Register(ComponentType type, ComponentPoolFactory factory);
	static ComponentPoolBase* CreatePool(ComponentType type);

private:
	static ComponentPoolFactory s_factories[COMPONENT_TYPE_COUNT];
};

struct ComponentPoolRegistrar
{
	ComponentPoolRegistrar(ComponentType type, ComponentPoolFactory factory)
	{
		ComponentRegistry::Register(type, factory);
	}
};

// Walks a pool skipping free slots; Get() returns NULL once past the last
// live component, so the idiomatic loop is:
//   for (ComponentIterator itr = manager.GetComponents(COMPONENT_X); itr.Get() != nullptr; itr++)
class ComponentIterator
{
public:
	explicit ComponentIterator(ComponentPoolBase* pool)
		: m_pool(pool)
		, m_idx(-1)
		, m_current(NULL)
	{
		Advance();
	}

	Component* Get() const { return m_current; }

	template <typename T>
	T* Get() const { return (T*)m_current; }

	ComponentIterator& operator++()
	{
		Advance();
		return *this;
	}

	ComponentIterator operator++(int)
	{
		ComponentIterator prev = *this;
		Advance();
		return prev;
	}

private:
	void Advance()
	{
		m_current = NULL;
		while (m_pool && ++m_idx < m_pool->Capacity())
		{
			Component* component = m_pool->GetBase(m_idx);
			if (component)
			{
				m_current = component;
				break;
			}
		}
	}

	ComponentPoolBase* m_pool;
	int m_idx;
	Component* m_current;
};

// Goes in the component's .cpp. Registers a factory for the type declared
// with DECLARE_COMPONENT before main() runs; each GameManager uses it to
// create its own pool instance for this type.
#define DEFINE_COMPONENT(className) \
	static ComponentPoolBase* Create##className##Pool() { return new ComponentPool<className, MAX_COMPONENTS>(); } \
	static ComponentPoolRegistrar s_##className##Registrar(className::TYPE, &Create##className##Pool);
