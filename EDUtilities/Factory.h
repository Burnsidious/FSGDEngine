#pragma once


namespace EDUtilities
{
    template<typename Type>
    class Factory
    {
    private:
        std::list<Type*> activeList;
        std::list<Type*> inactiveList;

        typedef void (*TypeInitializeFunction)(Type* objectPtr, void* voidPtr);
        typedef void (*TypeDeinitializeFunction)(Type* objectPtr, void* voidPtr);
        typedef void (*TypePostAllocFunction)(Type* objectPtr, void* voidPtr);

        Factory(const Factory&){}
        Factory& operator=(const Factory&){ return *this; }
    public:

        Factory(unsigned int count = 0)
        {
            for(unsigned int i = 0; i < count; ++i)
				inactiveList.push_back(new Type);
        }

        ~Factory(void)
        {
            while( activeList.empty() == false )
            {
				delete (*activeList.begin());
                activeList.pop_front();
            }

            while( inactiveList.empty() == false )
            {
				delete (*inactiveList.begin());
                inactiveList.pop_front();
            }
        }

        void Allocate(unsigned int count, TypePostAllocFunction func = 0, void* voidPtr = 0)
        {
            for(unsigned int i = 0; i < count; ++i)
            {
				Type* pType = new Type;

                if( func != 0 )
                    func( pType, voidPtr );

                inactiveList.push_back(pType);
            }
        }

        Type* GetNewObject(TypeInitializeFunction initFunc = 0, void* voidPtr = 0)
        {
            Type* newObjectPtr = 0;

            if( inactiveList.empty() )
            {
				// TODO: IS the double allocation intended?
                newObjectPtr = new Type;
                activeList.push_front( new Type );			
            }
            else
            {
                activeList.splice( activeList.begin(), inactiveList, inactiveList.begin() );
                newObjectPtr = (*activeList.begin());
            }

            if( initFunc != 0 )
                initFunc( newObjectPtr, voidPtr );

            return newObjectPtr;
        }

        Type* TryGetNewObject(TypeInitializeFunction initFunc = 0, void* voidPtr = 0)
        {
            if( inactiveList.empty() == false )
            {
                activeList.splice( activeList.begin(), inactiveList, inactiveList.begin() );

                if( initFunc != 0 )
                    initFunc( (*activeList.begin()), voidPtr );

                return (*activeList.begin());
            }
            else
                return 0;
        }

        void ReturnObject(Type* objectPtr, TypeDeinitializeFunction deinitFunc = 0, void* voidPtr = 0)
        {
            std::list< Type* >::iterator iter = activeList.begin();

            for(; iter != activeList.end(); ++iter)
            {
                if( (*iter) == objectPtr )
                {
                    inactiveList.splice( inactiveList.begin(), activeList, iter );

                    if( deinitFunc != 0 )
                        deinitFunc( (*inactiveList.begin()), voidPtr );

                    break;
                }
            }
        }
    };
}