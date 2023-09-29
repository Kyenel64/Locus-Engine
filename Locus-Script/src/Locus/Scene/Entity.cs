using System;
using System.Runtime.Versioning;

namespace Locus
{
    public class Entity
    {
        public readonly ulong ID; // UUID

        public Entity()
        {

        }

        internal Entity(ulong id)
        {
            ID = id;
        }

        public bool HasComponent<T>() where T : Component, new()
        {
            Type componentType = typeof(T);
            return InternalCalls.Entity_HasComponent(ID, componentType);
        }


        /*public T GetComponent<T>() where T : Component, new()
        {
            if (!HasComponent<T>())
                return null;

            T component = new T() { Entity = this };
            return component;
        }*/
    }
}
