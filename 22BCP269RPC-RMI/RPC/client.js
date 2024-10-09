const grpc = require('@grpc/grpc-js');
const protoLoader = require('@grpc/proto-loader');
const winston = require('winston'); // Import winston

const PROTO_PATH = './calc.proto';

const packageDefinition = protoLoader.loadSync(PROTO_PATH, {});
const calcProto = grpc.loadPackageDefinition(packageDefinition).Calculator;

// Winston logger setup for the client
const clientLogger = winston.createLogger({
    level: 'info',
    format: winston.format.combine(
        winston.format.timestamp(),
        winston.format.json()
    ),
    transports: [
        new winston.transports.File({ filename: 'client.log' }) // Logs written to client.log
    ]
});

const client = new calcProto('127.0.0.1:50051', grpc.credentials.createInsecure());

function performOperation() {
    const readline = require('readline');
    const rl = readline.createInterface({
        input: process.stdin,
        output: process.stdout
    });

    rl.question('Enter first number: ', (num1) => {
        rl.question('Enter second number: ', (num2) => {
            console.log('Select an operation:');
            console.log('1. Add');
            console.log('2. Subtract');
            console.log('3. Multiply');
            console.log('4. Divide');
            rl.question('Choose an option: ', (operation) => {
                const request = { num1: parseInt(num1), num2: parseInt(num2) };
                clientLogger.info(`User input: ${num1}, ${num2} | Selected operation: ${operation}`);

                switch (operation) {
                    case '1':
                        client.add(request, (err, response) => {
                            if (!err) {
                                console.log('Result:', response.result, response.message);
                                clientLogger.info(`Add result: ${response.result} | Message: ${response.message}`);
                            }
                        });
                        break;
                    case '2':
                        client.subtract(request, (err, response) => {
                            if (!err) {
                                console.log('Result:', response.result, response.message);
                                clientLogger.info(`Subtract result: ${response.result} | Message: ${response.message}`);
                            }
                        });
                        break;
                    case '3':
                        client.multiply(request, (err, response) => {
                            if (!err) {
                                console.log('Result:', response.result, response.message);
                                clientLogger.info(`Multiply result: ${response.result} | Message: ${response.message}`);
                            }
                        });
                        break;
                    case '4':
                        client.divide(request, (err, response) => {
                            if (!err) {
                                console.log('Result:', response.result, response.message);
                                clientLogger.info(`Divide result: ${response.result} | Message: ${response.message}`);
                            }
                        });
                        break;
                    default:
                        console.log('Invalid option');
                        clientLogger.warn('Invalid operation selected');
                }
                rl.close();
            });
        });
    });
}

performOperation();
