#pragma once

class GFX {
  public:
    GFX(){};
    virtual ~GFX(){};

    virtual CreateBuffer() = 0;
};