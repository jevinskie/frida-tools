var should_fetch_backtrace: boolean = true;
var exception_id: number = 0;

Process.setExceptionHandler(function (details) {
    console.log(`report-exceptions script got exception from JS. id: %${exception_id}`);
    send({'frida:exception-details': {'details': details, 'exception-id': exception_id}});
    var details_ack: boolean = false;
    while (!details_ack) {
        recv('frida:exception-details-ack', function (message, data) {
            details_ack = true;
        });
        Thread.sleep(0.000001); // Otherwise recv deadlocks. 1 us, works for g_usleep() on windows (Sleep(1)) and nanosleep elsewhere
    }
    if (should_fetch_backtrace) {
        console.log(`report-exceptions script sending backtrace details. id: ${exception_id}`);
        // let backtrace = Thread.backtrace(details.context, Backtracer.ACCURATE).map(DebugSymbol.fromAddress);
        let backtrace = ["<empty backtrace>"];
        send({'frida:exception-backtrace': {'backtrace': backtrace, 'exception-id': exception_id}});
        var backtrace_ack: boolean = false;
        while (!backtrace_ack) {
            recv('frida:exception-backtrace-ack', function (message, data) {
                backtrace_ack = true;
            });
            Thread.sleep(0.000001);
        }
    }
    ++exception_id;
    return false;
});

rpc.exports = {
    fetch_backtrace(enabled: boolean) {
        should_fetch_backtrace = enabled;
    }
};
