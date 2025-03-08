import axios from 'axios';

axios.post('http://localhost:8080', "Hello from frontend")
    .then(response => {
        console.log("Response from server:", response.data);
    })
    .catch(error => {
        console.error("Error:", error);
    });
