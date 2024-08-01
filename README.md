# PhysicsSystem

This is the physics engine I've written for use in a project I'm working on. Feel free to disect/play with as you please. It will likely change later on, and I currently have no plans on maintaining this repository long term.

### What does this do?

Why, it's a physics engine, of course! It's meant to be a lightweight engine for 2D collision. It has a system in place for keeping track of physics objects with 4 different collider classes/types:

- The base PhysicsBody. *This is the base PhysicsBody object that all PhysicsBody objects are created from.*
- RigidBody. *This is the physics body that is meant for movement. Drag, gravity, etc can be applied to this physics body. Best for character controllers.*
- StaticBody. *This is for static objects. You can apply movement to them (best use case is moving platforms), but they're mainly meant for objects that don't generally move. Walls, floors, etc.*
- Trigger. *This is meant for triggering interaction events between characters and the environment. e.g. triggers for doors, making objects in the environment interactable, loading the next level, etc.*

### What does this not do?

It is not a cure for cancer, loneliness, and it won't bring your ex back.

It's also not meant as an advanced physics engine for use with millions of objects (although you should be able to get away with a few hundred, if not a few thousand). It's not designed for 3D physics.