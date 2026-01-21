#pragma once

namespace core {

    class System
    {
    public:
        ~System() {}

        virtual void Update() {}
        virtual void Draw() {}
    };

}