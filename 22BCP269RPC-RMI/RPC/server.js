const grpc = require('@grpc/grpc-js');
const protoLoader = require('@grpc/proto-loader');
const winston = require('winston'); // Import winston

const PROTO_PATH = './calc.proto';

const packageDefinition = protoLoader.loadSync(PROTO_PATH, {
    keepCase: true,
    longs: String,
    enums: String,
    defaults: true,
    oneofs: true
});
const calcProto = grpc.loadPackageDefinition(packageDefinition).Calculator;

// Winston logger setup for the server
const serverLogger = winston.createLogger({
    level: 'info',
    format: winston.format.combine(
        winston.format.timestamp(),
        winston.format.json()
    ),
    transports: [
        new winston.transports.File({ filename: 'server.log' }) // Logs written to server.log
    ]
});

function add(call, callback) {
    const result = call.request.num1 + call.request.num2;
    serverLogger.info(`Add operation: ${call.request.num1} + ${call.request.num2} = ${result}`);
    callback(null, { result, message: "Addition successful" });
}

function subtract(call, callback) {
    const result = call.request.num1 - call.request.num2;
    serverLogger.info(`Subtract operation: ${call.request.num1} - ${call.request.num2} = ${result}`);
    callback(null, { result, message: "Subtraction successful" });
}

function multiply(call, callback) {
    const result = call.request.num1 * call.request.num2;
    serverLogger.info(`Multiply operation: ${call.request.num1} * ${call.request.num2} = ${result}`);
    callback(null, { result, message: "Multiplication successful" });
}

function divide(call, callback) {
    if (call.request.num2 === 0) {
        serverLogger.error(`Division by zero attempted: ${call.request.num1} / ${call.request.num2}`);
        callback(null, { result: 0, message: "Division by zero error" });
    } else {
        const result = call.request.num1 / call.request.num2;
        serverLogger.info(`Divide operation: ${call.request.num1} / ${call.request.num2} = ${result}`);
        callback(null, { result, message: "Division successful" });
    }
}

function main() {
    const server = new grpc.Server();

    server.addService(calcProto.service, {
        add: add,
        subtract: subtract,
        multiply: multiply,
        divide: divide
    });

    server.bindAsync('127.0.0.1:50051', grpc.ServerCredentials.createInsecure(), () => {
        console.log('gRPC server is running at http://127.0.0.1:50051');
        serverLogger.info('gRPC server started on port 50051');
        server.start();
    });
}

main();
