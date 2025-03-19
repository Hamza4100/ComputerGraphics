#include <iostream>
#include <fstream>
#include <string>


struct Pixel {
    unsigned char r, g, b;
};

struct Vertex {
    int x, y;
};


struct Face {
    int v1, v2, v3;  // Indices of the three vertices
    Pixel c1, c2, c3; // Colors at each vertex
};
// Allocate a 2D pixel array
Pixel** allocate(int h, int w) {
    Pixel** matrix = new Pixel * [h];
    for (int i = 0; i < h; i++) {
        matrix[i] = new Pixel[w];
    }
    return matrix;
}

// Deallocate memory
void deallocate(Pixel** matrix, int h) {
    for (int i = 0; i < h; i++) {
        delete[] matrix[i];
    }
    delete[] matrix;
}

// Extract numbers from a string
void extractNumbers(const std::string& line, int values[], int count) {
    int index = 0, num = 0;
    bool hasNum = false;

    for (char ch : line) {
        if (ch >= '0' && ch <= '9') {
            num = num * 10 + (ch - '0');
            hasNum = true;
        }
        else if (hasNum) {
            values[index++] = num;
            num = 0;
            hasNum = false;
            if (index >= count) break;
        }
    }
    if (hasNum) values[index] = num;
}

// Fill triangle with interpolated colors
void fillTriangle(Pixel** image, int x1, int y1, Pixel c1,
    int x2, int y2, Pixel c2,
    int x3, int y3, Pixel c3,
    int WIDTH, int HEIGHT) {
    int minX = std::max(0, std::min({ x1, x2, x3 }));
    int maxX = std::min(WIDTH - 1, std::max({ x1, x2, x3 }));
    int minY = std::max(0, std::min({ y1, y2, y3 }));
    int maxY = std::min(HEIGHT - 1, std::max({ y1, y2, y3 }));

    float denominator = float((y2 - y3) * (x1 - x3) + (x3 - x2) * (y1 - y3));
    if (denominator == 0.0f) return;

    for (int y = minY; y <= maxY; y++) {
        for (int x = minX; x <= maxX; x++) {
            float alpha = ((y2 - y3) * (x - x3) + (x3 - x2) * (y - y3)) / denominator;
            float beta = ((y3 - y1) * (x - x3) + (x1 - x3) * (y - y3)) / denominator;
            float gamma = 1.0f - alpha - beta;

            if (alpha >= -0.001f && beta >= -0.001f && gamma >= -0.001f) {
                unsigned char r = (unsigned char)(alpha * c1.r + beta * c2.r + gamma * c3.r);
                unsigned char g = (unsigned char)(alpha * c1.g + beta * c2.g + gamma * c3.g);
                unsigned char b = (unsigned char)(alpha * c1.b + beta * c2.b + gamma * c3.b);
                image[y][x] = { r, g, b };
            }
        }
    }
}

int main() {
    std::ifstream inputFile("input.txt");
    if (!inputFile) {
        std::cerr << "Error: Could not open input file!" << std::endl;
        return 1;
    }

    int WIDTH, HEIGHT;
    std::string line;

    // Read image size
    std::getline(inputFile, line); // # image size
    inputFile >> WIDTH >> HEIGHT;
    inputFile.ignore();
    inputFile.ignore(); // Ignore newline
    inputFile.ignore();
    // Read vertex list
    std::getline(inputFile, line); // # vertex list
    int vertexCount;
    inputFile >> vertexCount;
    inputFile.ignore(); // Ignore newline
    Vertex* vertices = new Vertex[vertexCount];

    for (int i = 0; i < vertexCount; i++) {
        inputFile >> vertices[i].x >> vertices[i].y;
    }
    inputFile.ignore(); // Ignore newline
    inputFile.ignore(); // Ignore newline
    // Read face list
    std::getline(inputFile, line); // # face list
    int faceCount;
    inputFile >> faceCount;
    inputFile.ignore(); // Ignore newline

    Face faces[100]; // Adjust size as needed (if dynamic, use `std::vector<Face>`)

    for (int i = 0; i < faceCount; i++) {
        inputFile >> faces[i].v1 >> faces[i].v2 >> faces[i].v3;
        std::getline(inputFile, line);

        int values[9];
        extractNumbers(line, values, 9);

        faces[i].c1 = { (unsigned char)values[0], (unsigned char)values[1], (unsigned char)values[2] };
        faces[i].c2 = { (unsigned char)values[3], (unsigned char)values[4], (unsigned char)values[5] };
        faces[i].c3 = { (unsigned char)values[6], (unsigned char)values[7], (unsigned char)values[8] };

        // Convert to 0-based indexing
        faces[i].v1--;
        faces[i].v2--;
        faces[i].v3--;
    }
    // Validate face indices after reading all faces
    for (int i = 0; i < faceCount; i++) {
        if (faces[i].v1 < 0 || faces[i].v1 >= vertexCount ||
            faces[i].v2 < 0 || faces[i].v2 >= vertexCount ||
            faces[i].v3 < 0 || faces[i].v3 >= vertexCount) {
            std::cerr << "Error: Face indices are out of range!" << std::endl;

            // Free allocated memory before exiting
            delete[] vertices;
            return 1;
        }
    }

    Pixel** image = allocate(HEIGHT, WIDTH);
    for (int y = 0; y < HEIGHT; ++y) {
        for (int x = 0; x < WIDTH; ++x) {
            image[y][x] = { 255, 255, 255 };
        }
    }
    for (int i = 0; i < faceCount; i++) {
        int v1 = faces[i].v1;
        int v2 = faces[i].v2;
        int v3 = faces[i].v3;

        Pixel c1 = faces[i].c1;
        Pixel c2 = faces[i].c2;
        Pixel c3 = faces[i].c3;

        fillTriangle(image,
            vertices[v1].x, vertices[v1].y, c1,
            vertices[v2].x, vertices[v2].y, c2,
            vertices[v3].x, vertices[v3].y, c3,
            WIDTH, HEIGHT);
    }


    std::ofstream imageFile("triangle.ppm", std::ios::out | std::ios::binary);
    if (!imageFile) {
        std::cerr << "Error: Could not open file for writing!" << std::endl;
        delete[] vertices;
        deallocate(image, HEIGHT);
        return 1;
    }
    imageFile << "P6\n" << WIDTH << " " << HEIGHT << "\n255\n";
    for (int y = 0; y < HEIGHT; ++y) {
        imageFile.write(reinterpret_cast<char*>(image[y]), WIDTH * 3);
    }

    imageFile.close();
    delete[] vertices;
    deallocate(image, HEIGHT);
    std::cout << "PPM image generated: triangle.ppm" << std::endl;

    return 0;
}
