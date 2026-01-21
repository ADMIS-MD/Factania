#pragma once

namespace core {

    class System
    {
        ~System() {}

        virtual void Update() {}
        virtual void Draw() {}
    };

}