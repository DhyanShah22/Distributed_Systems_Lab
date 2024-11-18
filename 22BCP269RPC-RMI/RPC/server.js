const grpc = require('@grpc/grpc-js');
const protoLoader = require('@grpc/proto-loader');

const PROTO_PATH = './calc.proto';

const packageDefinition = protoLoader.loadSync(PROTO_PATH, {});
const calcProto = grpc.loadPackageDefinition(packageDefinition).Calculator;

const add = (call, callback) => {
    const result = call.request.num1 + call.request.num2;
    callback(null, {result, message: "Addition succesful"})
}

const subtract = (call, callback) => {
    const result = call.request.num1 - call.request.num2;
    callback(null, { result, message: "Subtraction successful" });
};

const multiply = (call, callback) => {
    const result = call.request.num1 * call.request.num2;
    callback(null, { result, message: "Multiplication successful" });
};

const divide = (call, callback) => {
    if (call.request.num2 === 0) {
        callback(null, { result: 0, message: "Cannot divide by zero" });
    } else {
        const result = call.request.num1 / call.request.num2;
        callback(null, { result, message: "Division successful" });
    }
};

const main = () => {
    const server = new grpc.Server();
    server.addService(calcProto.service, { add, subtract, multiply, divide });
    server.bindAsync('127.0.0.1:50051', grpc.ServerCredentials.createInsecure(), () => {
        console.log('Server running at http://127.0.0.1:50051');
        server.start();
    });
};

main();
