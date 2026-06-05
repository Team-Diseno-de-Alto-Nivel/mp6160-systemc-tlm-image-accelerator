#include <systemc>

// Smoke-test module — replace with actual system modules when implementing.
SC_MODULE(HelloWorld) {
    SC_CTOR(HelloWorld) {
        SC_THREAD(run);
    }

    void run() {
        std::cout << "Hello, SystemC! t=" << sc_core::sc_time_stamp() << "\n";
    }
};

int sc_main(int argc, char* argv[]) {
    HelloWorld hw("hw");
    sc_core::sc_start();
    return 0;
}
