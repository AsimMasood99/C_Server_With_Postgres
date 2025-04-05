DROP TABLE IF EXISTS Users;

CREATE TABLE Users (
    id SERIAL PRIMARY KEY,
    Username VARCHAR(20),
    Password INT
);

INSERT INTO Users (Username, Password) VALUES 
('Asim', 123),
('Ahmad', 345),
('Ali', 567);
