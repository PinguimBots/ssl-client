#pragma once

namespace pbts {

    class Strategy
    {
    public:
        Strategy(bool isYellow);
        int pertoBola();

    private:
        const bool _isYellow;
    };

}
