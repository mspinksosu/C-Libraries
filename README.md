# C-Libraries

A collection of C libraries I've written targeted towards microcontrollers, using C99 standards.

---
#### Status
A lot of these work just fine, but I haven't finished fully documenting them, or I haven't done the PIC implementaion yet so I don't consider those done yet.

- [ ] ADC: Interface and generic manager complete! Needs field testing
  - [x] STM32 implementation finished!
  - [x] STM32 DMA implementation finished! Testing in progress
  - [x] Update doxygen
  - [ ] PIC32 implementation
- [x] Bitfield: Complete and tested!
- [x] Buffer: Complete!
- [x] Button: Refactored! 99% tested
  - [x] Added analog button
  - [x] Update doxygen
- [x] Button Group: Tested and working!
- [x] Comparator: Tested and working!
  - [x] Update doxygen
- [ ] Filter: Added two basic classes, SMA and EMA
  - [x] Interface
  - [ ] Documentation
- [ ] GPIO: Redesigned!
  - [x] STM32 implementation finished! Testing in progress
  - [x] Refactored STM32 G0 to get rid of ST library dependancy. Needs re-testing
  - [x] Update doxygen
  - [ ] PIC32 implementation
- [ ] Graphics: New library! (to go with TFT display eventually)
  - [ ] Line drawing algorithm working! Needs testing on octants 2, 3, 6, 7
  - [x] Simple number to ascii string
- [ ] Hardware Timer: New library! Ready to start testing
  - [ ] Add more STM32 sub class options
  - [ ] Documentation
- [ ] I2C: Old PIC32 library. Worked before. Needs updating
- [ ] LCD: New library! Ready to start testing
  - [ ] 4-bit mode
  - [ ] Screen scroll
  - [ ] Documentation
- [ ] MCU: Scheduler tested working!
  - [ ] Documentation
- [x] Pattern: Tested and working!
  - [x] Update doxygen
- [ ] Rotary Encoder: Redesigned! Testing in progress
  - [x] Add different types of rotary encoder
- [ ] SPI: Interface and generic manager complete! Ready to start testing STM32
    - [x] Basic STM32 implementation ready to start testing
    - [x] SPI Manager basic state machine for master mode
    - [ ] PIC32 implementation
    - [ ] Documentation
- [x] Switch: Complete!
- [x] Timer: Complete!
- [ ] UART: STM32 tested and working!
    - [x] STM32 G0 implementation finished! Testing in progress
    - [x] Added options for flow control and interrupts
    - [x] STM32 F1 implementation tested and working!
    - [x] PIC16 implementation finished! Testing in progress
    - [x] Update doxygen
    - [ ] PIC32 implementation

---
### Introduction

Almost all of my libraries use objected orientated programming techniques. You will often see functions that contain a parameter called **"self"**. This is the object, or class, that is being modified. This is just a keyword that I borrowed from Objective C. But in C, we must pass a reference to the object, or in this case, the struct into the function. In Java, the assignment of this first parameter "self" is done automatically for you. It's still there, it's just done behind the scenes when the method is called. Think of "self" as being the same as "this" in Java. The only difference is we must access it with a special arrow notation inside the function rather than the dot notation. Like so: **"self->width = x;"** is similar to **"this.width = x;"** in Java.

### Naming Conventions

Almost all of my libraries use some sort of OOP style. The first parameter in the function prototype is always the object being used (self) followed by any other parameters. Any interfaces use the prefix "I" in their name. Function names will have a prefix with an underscore to indicate which library they belong to. For example, the UART library interface is called **"IUART.h"** and all functions begin with **"UART_"**. Further separation of implementations is done by prefixing them to the function. For example **"ADC_STM32G0_GetValue"** is decoded like so: **"ADC_"** (library name or interface) + **"STM32G0_"** (processor implementation) + **"GetValue"** (function name)

### Types of Libraries

I have a few different types of libraries here. I will try my best to briefly describe them in my own words.

* Standard Library
* Simple Interface
* Library With Combined Classes
* Interface With Polymorphism

#### Standard Library

A library where everything is self contained and there is no processor specific code or interface needed. **Examples: Buffer, Timer, Switch**

#### Simple Interface

In this library there is a header file which plays the role of the interface (prefixed with an "I"). There may be one or more .c files that can be used with this header file. These .c files (implementations) have all the function names listed in the header file, but each may contain their own processor specific code and dependencies. Multiple .c files can be created and added to a project, but only **one** may be used at a time. You can change the implementation by swapping the .c files in and out. This type of interface is useful when building libraries for basic peripherals. As long as everything can be done with the standard c data types like uint8_t and bool this type of interface is a good choice. **Examples: ADC, UART, SPI, I2C**

I took this a step further recently and added a function table to my UART library. This is just so that I can make the calling functions the same across different processors. For example, BT_UART could be UART1 on my PIC16, but UART2 on my STM32. In reality, the simple interface still exists. The UART1 interface (header file) exists in both the PIC16 and STM32 and each has still have their own implementation. For these types of libraries, you can set the function table first and then call the init function. This is because there is no inheritance going on, so there are no sub class specific init functions to call. More on that below.

#### Library With Combined Classes

A library with one or more classes that inherit from a base class. All classes are contained within a single .h and .c file. There will be some sort of type qualifier that determines which function to call. When a function is called, it looks at the type of object it has been given, and calls the appropriate implementation for that object. This is useful when different types of objects have differing types or number of variables, and combining all the variables in a single struct would be a waste of memory. Best used for small classes. A good example would be a button with a digital and analog type. Each type needs different variables to get the job done. **Example: Button**

#### Interface With Polymorphism

In this library there is a base class and one or more sub classes. There is a .h and .c file both prefixed with "I". The "I" .h file declares the base class object and the "I" .c file handles the function calls. Although I call them the "interface", the actual role of interfaceing is really done by the **function table** in the header file. In these examples **function table** and **interface** and are really just the same thing. It's sometimes also referred to as a **vtable**. The base class is in the "I" header file. It contains a void pointer to the sub class and any base class variables needed. The sub class object inherits from this base class. The sub class must also create its own function table which contains the functions listed in the interface. To use this type of library, you make two objects, one of the base class and one of the sub class. Then the two are linked together along with the function table. This is similar to how C++ does it, but in C++ it's all done behind the scenes. The object now contains a pointer to the base class, it's own variables, and a pointer to the function table. You call a base class function and give it a reference to your object. The function table contained within the object now determines which implementation of the function to call. This is great for implementations which need different types or numbers of objects and variables. Study the template files to see how it works. **Examples: MapFunction, GPIO, UART**

For this type of library, my preferred method is to make the function table static for each implementation, and call the implementation's create function. From there, I set the function table. More info at the bottom (typecasting).

### Commandments For Using My Libraries

#### Thou shall not have dependencies!

Interfaces are basically a template to follow for a library. Interfaces do not contain processor specific code or dependencies. Things like *"p32mz0512efe064.h"* or *"stm32g071xx.h"* do not belong here! Processor specific code goes in the **implementation**. Think of the implementation as how each particular processor chooses to do the things that are laid out in the interface. If you have a bunch of #includes in your code, you probably have dependencies. When you go to port your code to a different processor, you might make your life difficult.

#### Thou shall use getters and setters

You should not have to manipulate data in a class directly. Whenever possible I have provided getters and setters to do this for you. Yes, I understand this is C and there are no private variables. But trust me. Just use the functions. What if the data types for different processors have different variables? If you try to port your code, it could break.

#### Thou shall not use excessive global variables

>"By the way, there is a tendency to make everything in sight an extern variable because it appears to simplify communications-argument lists are short and variables are always there when you want them. But external variables are always there even when you don't want them. Relying too heavily on external variables is fraught with peril since it leads to programs whose data connections are not at all obvious-variables can be changed in unexpected and even inadvertent ways, and the program is hard to modify." - Dennis M. Ritchie, The C Programming Language

#### Thou shall not use excessive \#define's and \#ifdef's

Let's say you made an \#ifdef on a function call somewhere. Now you have to apply that same \#ifdef in multiple places throughout your code. The whole reason we are making interfaces is so that we never need to change something in multiple places in our code. The only time I feel an \#ifdef is useful is changing out the sub class declaration for a different one. However, the calling function will remain the same. Be careful of putting a bunch of \#defines all over the place too. It would be really annoying to try and port some code just to find that you're missing a random \#define that was in some file. Limit your \#define's to their relevant .c files as much as you can.

#### Love thy function pointers

I love function pointers. You should too. They can be useful for getting rid of dependencies. Let's say you have Bluetooth module you want to interface with. Our theoretical Bluetooth module could use UART or SPI. Having a function pointer is great, because now you can simply define a "transmit byte" and "receive byte" function pointer for the BT module interface. And you can connect those functions to your UART or SPI libraries and remove the UART and SPI dependencies from your interface code. The BT module could set a pin high on wake to signal that it is ready. You can have a function pointer in your interface called "wake event" that the interface calls whenever it needs to read a GPIO pin, or it gets a series of bytes. This is another very common type of function pointer called a **callback** function. This all goes back to the first commandment, which is *no dependencies*. ***There is one very important rule when using function pointers. Always check if the function pointer is not NULL before calling it.***

#### Words of wisdom

Before you go including a header file to something, remember to always ask yourself this important question: ***"Does this thing really need to know about what is calling it?"*** In other words, does my BT module care about how data is transmitted? Could it be UART, SPI, or something else? Does my button library need to know about what kind of GPIO is used? Or here is another one: ***"What happens if there is a different type of thing I'm interfacing with? Does it break my code?"*** What if the button is on an external board with an shift register?. If the answer to the above questions is *"No my BT module doesn't need to know how it gets its bytes"* or *"My library needs code to operate a shift register"* **then that code doesn't belong in there. Get rid of those dependencies.** Yes, writing interfaces and setting up multiple function pointers is tedious. But it's usually only needed once. And seeing the code you wrote for a PIC16 compile and run on a STM32 with no changes is *extremely* satisfying.

---
**A word about typecasting:** I've tried multiple methods for polymorphism, and I've settled on a blend of different methods and the use of a void pointer. The reason for the void pointer is that it avoids the user always needing to typecast with every function call. (Remember you are calling a function with a base class signature but you need to give it a sub class) However, void pointers can be difficult to use and even dangerous. In order to make it more type-safe, I place the create function inside of a sub class create function. The function signature for the sub class requires both a base class and a sub class object. 

**A word about malloc:** My C libraries are targeted towards microcontrollers. Many small microcontrollers like PIC16's do not make use of malloc. So I have chosen not to use it. This means that things like declaring arrays, base classes, and sub classes are done out in the open. This may look a little more confusing at first, because of the long function signatures. But it's okay. If we used had malloc, then when we call a base class or sub class initialization, the creation of the object and declaration is inside the function. We're essentially doing the same thing. The only differences is we are declaring our objects before calling the initialization functions. This may seem like a big limitation, but most of these libraries are geared towards hardware peripherals, so creation of objects is normally only needed once. And you rarely ever un-itialize or free memory for a periperhal that you are using. The sub class initialization function will usually contain both types of objects. For example: **void Foo_MCU1_Init(Foo_MCU1 *self, Foo *base, uint16_t data)**. has two parameters. The base class **Foo** and the sub class **Foo_MCU1**. Notice how Foo_MCU1 is listed first. That's because function is for the sub class **Foo_MCU1** as you can see by the prefixes. So the sub class parameter is listed first, then the base class, then any relevent data.







