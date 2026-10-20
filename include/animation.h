#pragma once

class AnimationManager {
    public:
    Uint64 StoreTickMs();

    Uint64 mStoredTick {0};
};