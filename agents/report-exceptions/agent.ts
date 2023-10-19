var yield_name: string;
var yield_module: string;
var yield_res_type: NativeFunctionReturnType;
let platform = Process.platform;
if (platform == 'windows') {
    yield_name = 'SwitchToThread';
    yield_module = 'Kernel32.dll';
    yield_res_type = 'bool';
} else {
    yield_name = 'sched_yield';
    yield_res_type = 'int';
    if (platform == 'darwin') {
        yield_module = '/usr/lib/system/libsystem_pthread.dylib';
    } else {
        yield_module = 'libc.so';
    }  
}
let yield_addr = Module.getExportByName(yield_module, yield_name);
let yield_native = new NativeFunction(yield_addr, yield_res_type, []);

function exception_handler(details: ExceptionDetails): boolean {
    console.log('report-exceptions script got exception from JS');
    // let backtrace = Thread.backtrace(details.context, Backtracer.ACCURATE).map(DebugSymbol.fromAddress);
    let backtrace = "<empty backtrace>";
    send({'frida:exception': {'details': details, 'backtrace': backtrace}});
    var ack: boolean = false;
    while (!ack) {
        recv('frida:exception-ack', function (message, data) {
            ack = true;
        });
        let yield_res = yield_native();
        if (yield_res) {
            throw new Error(`yield_native returned non-zero value: ${yield_res}`);
        }
    }
    return false;
}

Process.setExceptionHandler(exception_handler);

rpc.exports = {
    init() {
        return true;
    }
};
