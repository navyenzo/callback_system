#ifndef CALLBACKS_HPP
#define CALLBACKS_HPP



///*****************************************************************************
///*****************************************************************************
///
///
///
/// These generic template classes help programmers add callback functions
/// to their custom classes, providing register/invoke/de-register functions
/// with callback IDs for easy de-restering of callbacks
///
///
///
/// -- These classes define different types of callbacks:
///
///    1.  Callbacks that return a boolean which is true if it
///        successfully understood and worked on the arguments or
///        false otherwise
///
///    2.  Callbacks that return a container that defines the
///        empty() function, so that we can test whether the
///        callback successfully understood and worked on the
///        arguments or no
///
/// -- The classes provide two algorithms to invoke the callbacks:
///
///    1.  The first  algorithm invokes the added callbacks going through the
///        callbacks sequentially, one at a time.  As soon as one callback
///        successfully understands and works on the input arguments, the
///        algorithm returns without invoking the rest of the callbacks
///
///    2.  The second algorithm invokes all the added callbacks, not caring
///        about whether the callbacks successfully understand and work on
///        the input arguments or not
///
///
///
/// Note: This class is defined within the namespace CallbacksLIB
///
///
///
/// Created by:     Vincenzo Barbato
/// email:          navyenzo@gmail.com
///
///
///
///*****************************************************************************
///*****************************************************************************



//-------------------------------------------------------------------
// Includes needed for this class
//-------------------------------------------------------------------
#include <functional>
#include <vector>
#include <atomic>
//-------------------------------------------------------------------



//-------------------------------------------------------------------
// Classes are defined within the namespace CallbacksLIB
//-------------------------------------------------------------------
namespace CallbacksLIB
{
//-------------------------------------------------------------------



//-------------------------------------------------------------------
// Class used to pair a callback function with an ID to allow
// de-registering callbacks
//-------------------------------------------------------------------
template<typename CallbackReturnType,
         typename...CallbackArguments>

class Callback
{
public: // Public typedefs



    using CallbackFunctionType = std::function<CallbackReturnType(CallbackArguments...arguments)>;



public: // Constructors and destructor



    Callback(){}
    ~Callback(){}



public: // Operator and function used to invoke the callback



    CallbackReturnType          operator()(CallbackArguments...arguments)
    {
        return m_callback(arguments...);
    }



    CallbackReturnType          operator()(CallbackArguments...arguments)const
    {
        return m_callback(arguments...);
    }



public: // Public variables



    // The callback ID used to de-register callbacks

    int                         m_id = 0;



    // The actual function invoked when invoking
    // this callback

    CallbackFunctionType        m_callback;
};
//-------------------------------------------------------------------



//-------------------------------------------------------------------
// Class defining a "callback system" which is made of a vector
// that holds "registered callbacks"
//
// The system allows programmers to "register", "invoke" and
// "deregister" callback functions
//-------------------------------------------------------------------
template<typename CallbackReturnType,
         typename...CallbackArguments>

class Callbacks
{
public: // Public typedefs



    using CallbackFunctionType = std::function<CallbackReturnType(CallbackArguments...arguments)>;
    using CallbackType = Callback<CallbackReturnType,CallbackArguments...>;




public: // Constructors and destructor



    // Default constructor

    Callbacks(){}



    // Destructor

    ~Callbacks(){}



public: // Public functions



    // Function used to register a callback

    int register_callback(CallbackFunctionType callback)
    {
        CallbackType newCallback;

        newCallback.m_id = (++m_lastAssignedCallback_ID);
        newCallback.m_callback = callback;

        m_callbacks.push_back(newCallback);

        return newCallback.m_id;
    }



    // Function used to de-register a callback

    bool deregister_callback(const int& callbackID)
    {
        for(int i = 0; i < m_callbacks.size(); ++i)
        {
            if(m_callbacks[i].m_id == callbackID)
            {
                m_callbacks.erase(m_callbacks.begin() + i);
                return true;
            }
        }

        return false;
    }



    // Function used to de-register all callbacks

    void deregister_all_callbacks()
    {
        m_callbacks.clear();
    }



    // Function invoking all the callbacks

    CallbackReturnType invokeCallbacks(CallbackArguments...arguments)const
    {
        for(const auto& callback : m_callbacks)
        {
            callback(arguments...);
        }
    }



public: // Public operator() used to invoke all
        // the callbacks with the specified arguments



    CallbackReturnType operator()(CallbackArguments...arguments)const
    {
        for(const auto& callback : m_callbacks)
        {
            callback(arguments...);
        }
    }



protected: // Protected variables



    // The vector holding the callbacks
    // that have been added

    std::vector<CallbackType>           m_callbacks;



    // The ID used to identify each
    // added callback to allow users
    // to de-register them at a later
    // time
    //
    // NOTE:  It is made atomic to allow
    //        multiple threads to register
    //        callbacks while being assigned
    //        a unique id


    std::atomic<int>                    m_lastAssignedCallback_ID;
};
//-------------------------------------------------------------------



//-------------------------------------------------------------------
// Specialization that invokes the callbacks but returns as soon as
// one of them returns a non-empty container
//
// This specialization assumes the return type is a container that
// defines the empty() member function
//
// Callbacks are invoked sequentially until one of them returns a
// non-empty container
//-------------------------------------------------------------------
template<typename CallbackReturnType,
         typename...CallbackArguments>

class CallbacksReturningAContainer : public Callbacks<CallbackReturnType,CallbackArguments...>
{
public: // Constructors and destructor



    // Default constructor

    CallbacksReturningAContainer() : Callbacks<CallbackReturnType,CallbackArguments...> (){}



    // Destructor

    ~CallbacksReturningAContainer(){}



public: // Public functions



    // Function invoking all the callbacks but
    // returning as soon as a callback returns
    // a non-empty container

    CallbackReturnType invokeCallbacksUntilOneOfThemReturnsANonEmptyContainer(CallbackArguments...arguments)const
    {
        CallbackReturnType callbackReturn;

        for(const auto& callback : this->m_callbacks)
        {
            callbackReturn = callback(arguments...);

            if(!callbackReturn.empty())
                return callbackReturn;
        }

        return callbackReturn;
    }
};
//-------------------------------------------------------------------



//-------------------------------------------------------------------
// Specialization that invokes the callbacks but returns as soon as
// one of them returns a non-zero value (like a boolean true)
//
// This specialization assumes the return type can be checked like
// a boolean in an if-statement
//-------------------------------------------------------------------
template<typename...CallbackArguments>

class CallbacksReturningABoolean : public Callbacks<bool,CallbackArguments...>
{
public: // Public typedefs



    using CallbackFunctionType = typename Callbacks<bool,CallbackArguments...>::CallbackFunctionType;



public: // Constructors and destructor



    // Default constructor

    CallbacksReturningABoolean() : Callbacks<bool,CallbackArguments...> (){}



    // Destructor

    ~CallbacksReturningABoolean(){}



public: // Public functions



    // Function invoking all the callbacks but
    // returning as soon as a callback returns
    // a non-zero value (like a boolean true)

    bool invokeCallbacksUntilOneOfThemReturnsANonZeroValue(CallbackArguments...arguments)const
    {
        for(const auto& callback : this->m_callbacks)
        {
            if(callback(arguments...))
                return true;
        }

        return false;
    }
};
//-------------------------------------------------------------------



//-------------------------------------------------------------------
// End of CallbacksLIB namespace
//-------------------------------------------------------------------
}
//-------------------------------------------------------------------



#endif // CALLBACKS_HPP
