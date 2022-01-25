#include <iomanip>
class reader : public std::ifstream {

public:

    // Constructor
    template <class... Args>
    inline reader(int max, Args&&... args) :
    std::ifstream(args...), _max(max), _last(0) {
        if (std::ifstream::is_open()) _measure();
    }

    // Opens the file and measures its length
    template <class... Args>
    inline auto open(Args&&... args)
    -> decltype(std::ifstream::open(args...)) {
        auto rvalue(std::ifstream::open(args...));
        if (std::ifstream::is_open()) _measure();
        return rvalue;
    }

    // Displays the progress bar (pos == -1 -> end of file)
    inline void drawbar(void) {

        int pos(std::ifstream::tellg());
        float prog(pos / float(_length)); // percentage of infile already read
        if (pos == -1) { _print(_max + 1, 1); return; }

        // Number of #'s as function of current progress "prog"
        int cur(std::ceil(prog * _max));
        if (_last != cur) _last = cur, _print(cur, prog);

    }

private:

    std::string _inpath;
    int _max, _length, _last;

    // Measures the length of the input file
    inline void _measure(void) {
        std::ifstream::seekg(0, end);
        _length = std::ifstream::tellg();
        std::ifstream::seekg(0, beg);
    }

    // Prints out the progress bar
    inline void _print(int cur, float prog) {

        std::cout << std::fixed << std::setprecision(2)
            << "\r   [" << std::string(cur, '#')
            << std::string(_max + 1 - cur, ' ') << "] " << 100 * prog << "%";

        if (prog == 1) std::cout << std::endl;
        else std::cout.flush();

    }

};

