const grpc = require('@grpc/grpc-js');
const protoLoader = require('@grpc/proto-loader');

const PROTO_PATH = './calc.proto';

const packageDefinition = protoLoader.loadSync(PROTO_PATH, {});
const calcProto = grpc.loadPackageDefinition(packageDefinition).Calculator;

const readline = require('readline')
rl = readline.createInterface({
    input: stdin,
    output: stdout
})


const client = new calcProto('127.0.0.1:50051', grpc.credentials.createInsecure());

const performOperation = (operation, num1, num2) => {
    const request = { num1, num2 };
    client[operation](request, (err, response) => {
        if (err) {
            console.error('Error:', err.message);
        } else {
            console.log('Result:', response.result, '| Message:', response.message);
        }
    });
};

rl.question('Enter first number: ', (num1) => {
    rl.question('Enter second number: ', (num2) => {
        console.log('Select an operation: add, subtract, multiply, divide');
        rl.question('Enter operation: ', (operation) => {
            performOperation(operation, parseInt(num1), parseInt(num2));
            rl.close();
        });
    });
});
