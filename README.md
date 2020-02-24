# callback_system
` `  
A c++ library that makes it easy to add "generic" callback functions to classes, with the abillity to:
` `  
*  Register callback functions at compile time or runtime
*  De-register callback functions at compile time or runtime
*  Invoke callback functions in the following two ways:
    *  Invoke all the registered callback functions sequentially
    *  Invoke all the registered callback functions sequentially but stop as soon as one of the functions returns a valid value
*  Register any kind of callback:
    *  Stand alone functions
    *  Member functions
    *  Functors
*  Register functions with a custom return type and a variable number of arguments (Decided at compile time)
` `  
# How to use
` `  
The library is simple to use and add to existing code.
` `  
The following example shows how to add multiple callbacks to an existin class in c++
` `  
First we show the minimal code you need to add to your existing class to give it the abilities to add callbacks:
` `  
```cpp

#include "callback_system/callbacks.hpp"



//-------------------------------------------------------------------
// Some example class (we want to add multiple callbacks to this class)
//-------------------------------------------------------------------
class ExampleClass
{
public: // Public typedefs (make it easier to add callbacks)



    // Example callback system (multiple callbacks) that use
    // functions which return a "bool" and take as arguments "(const char*,int)"

    using CallbackSystemType = CallbacksLIB::Callbacks<bool,const char*, int>;



public: // Constructors and destructor



    // Default constructor

    ExampleClass();



    // Destructor

    ~ExampleClass();



public: // Public functions



    // Function used to get the callback system so that the user can
    // register/deregister callbacks from this class

    CallbackSystemType&                     callbacks();



protected: // Protected variables



    // The callbacks

    CallbackSystemType                      m_callbacks;
};
//-------------------------------------------------------------------

```
` `  
` `  
Second we show how to register/deregister callback functions in objects defined by the class above:
` `  
```cpp

//-------------------------------------------------------------------
// Here's some function we want to 
// Default UDP Message handling callback
//-------------------------------------------------------------------
inline bool anExampleFunctionThatWeAssignAsCallback(const char* message, int sizeOfMessageInBytes)
{
    // Do something with message

    return true;
}
//-------------------------------------------------------------------



//-------------------------------------------------------------------
// main function
//-------------------------------------------------------------------
int main(int argc, char** argv)
{
    // Creating the object

    ExampleClass exampleObject;



    // Registering the callback

    auto callbackID = exampleObject.callbacks().register_callback(std::bind(anExampleFunctionThatWeAssignAsCallback,
                                                                            std::placeholders::_1,
                                                                            std::placeholders::_2));



    // NOTE:  Callback IDs can be used to de-register
    //        a callback from the system

    // Remove callback (de-register)

    exampleObject.callbacks().deregister_callback(callbackID);



    return 0;
}
//-------------------------------------------------------------------

```
