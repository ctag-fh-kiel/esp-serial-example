const SerialPort = require('serialport');
const port = new SerialPort('/dev/tty.SLAB_USBtoUART', {
    baudRate: 115200
});
const Readline = SerialPort.parsers.Readline;
const parser = port.pipe(new Readline());

// send dummy data
setInterval(()=>{
    let cmd = {
        api: "api/v1",
        cmd : "getPlugins",
        params: ""
    };
    port.write(JSON.stringify(cmd) + '\n', function(err) {
        if (err) {
            return console.log('Error on write: ', err.message);
        }
    })
}, 1000);

// receive data from esp echo
parser.on('data', data => {
    const regex = /\{(.*?)\}/gi;
    let parsed_data = data.match(regex);
    if(parsed_data){
        //console.log(parsed_data);
        // parse from JSON
        let jsn = JSON.parse(parsed_data.pop());
        console.log('API: ', jsn.api);
        console.log('CMD: ', jsn.cmd);
    }else{
        console.log(data);
    }
});