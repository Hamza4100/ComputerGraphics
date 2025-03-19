#include <iostream>
#include <fstream>
#include <string>

using namespace std;

struct Pixel {
    unsigned char r, g, b;
};

Pixel** allocate(int h, int w) {
    Pixel** matrix = new Pixel * [h];
    for (int i = 0; i < h; i++) {
        matrix[i] = new Pixel[w];
    }
    return matrix;
}

void free(Pixel** matrix, int height) {
    for (int i = 0; i < height; i++) {
        delete[] matrix[i];
    }
    delete[] matrix;
}

bool read(const string& filename, Pixel**& img, int& w, int& h) {
    ifstream file(filename, ios::binary);
    if (!file) {
        cout << "Error: Can not open file " << filename << "!" << endl;
        return false;
    }

    string line;
    file >> line;
    if (line != "P6") { 
        cout << "Error: Only P6 format supported!" << endl;
        return false;
    }

    while (file >> ws && file.peek() == '#') {  // `ws` removes  whitespace and peek() checks next index{refrence:provided in Documentation}
        getline(file, line);  
    }

    int maxColor;
    file >> w >> h >> maxColor;
    file.ignore(); 

    if (w <= 0 || h <= 0 || maxColor != 255) {
        cout << "Error: Invalid image format!" << endl;
        return false;
    }

    img = allocate(h, w);

    for (int i = 0; i < h; i++) {
        file.read(reinterpret_cast<char*>(img[i]), w * 3);  // cast for converting Pixel to char pointer 
        if (!file) {
            cout << "Error: Unexpected EOF or corrupt data!" << endl;
            free(img, h);
            return false;
        }
    }

    file.close();
    return true;
}


bool write(const string& f_name, Pixel** img, int w, int h) {
    ofstream file(f_name, ios::binary);
    if (!file) {
        cout << "Error: Can not create file " << f_name << "!" << endl;
        return false;
    }

    file << "P6\n" << w << " " << h << "\n255\n";

    for (int i = 0; i < h; i++) {
        file.write(reinterpret_cast<char*>(img[i]), w * 3); // cast for converting Pixel to char pointer 
    }

    file.close();
    return true;
}

void Gray(Pixel** image, int w, int h) {
    for (int i = 0; i < h; i++) {
        for (int j = 0; j < w; j++) {
            unsigned char gray = 0.299 * image[i][j].r + 0.587 * image[i][j].g + 0.114 * image[i][j].b; // provided refrence in documentation
            image[i][j].r = image[i][j].g = image[i][j].b = gray;
        }
    }
}

void Negative(Pixel** img, int w, int h) {
    for (int i = 0; i < h; i++) {
        for (int j = 0; j < w; j++) {
            img[i][j].r = 255 - img[i][j].r;
            img[i][j].g = 255 - img[i][j].g;
            img[i][j].b = 255 - img[i][j].b;
        }
    }
}

void Subtract(Pixel** img1, Pixel** img2, Pixel** result, int w, int h) {
    for (int i = 0; i < h; i++) {
        for (int j = 0; j < w; j++) {
            result[i][j].r = max(0, img1[i][j].r - img2[i][j].r);
            result[i][j].g = max(0, img1[i][j].g - img2[i][j].g);
            result[i][j].b = max(0, img1[i][j].b - img2[i][j].b);
        }
    }
}

void Combine(Pixel** img1, Pixel** img2, Pixel** result, int w, int h, float alpha) {
    for (int i = 0; i < h; i++) {
        for (int j = 0; j < w; j++) {
            result[i][j].r = alpha * img1[i][j].r + (1 - alpha) * img2[i][j].r;
            result[i][j].g = alpha * img1[i][j].g + (1 - alpha) * img2[i][j].g;
            result[i][j].b = alpha * img1[i][j].b + (1 - alpha) * img2[i][j].b;
        }
    }
}


void Red(Pixel** img, int w, int h) {
    for (int i = 0; i < h; i++) {
        for (int j = 0; j < w; j++) {
            img[i][j].g = 0; 
            img[i][j].b = 0;  
        }
    }
}

void Green(Pixel** img, int w, int h) {
    for (int i = 0; i < h; i++) {
        for (int j = 0; j < w; j++) {
            img[i][j].r = 0;  
            img[i][j].b = 0;  
        }
    }
}


void Blue(Pixel** img, int w, int h) {
    for (int i = 0; i < h; i++) {
        for (int j = 0; j < w; j++) {
            img[i][j].r = 0; 
            img[i][j].g = 0;  
        }
    }
}

int main() {
    int w, h;
    Pixel** img = nullptr;
    Pixel** img2 = nullptr;
    Pixel** img3 = nullptr;
    string f_name;
    char OuterSwitchOP = '\0';
    char InnerSwitchOP = '\0';
    bool done = false;
    while (OuterSwitchOP != '7') {
        cout << "**********************IMAGE PROCESSING SYSTEM******************"<<endl;
        cout << "\nChoose an option:\n";
        cout << "1. Create Negative\n2. Create Gray\n3. Create Morph \n4. Combine Images\n5. Subtract\n6. Color image\n7. Quit\n";
        cout << "Enter your choice: ";
        cin >> OuterSwitchOP;
        cin.ignore();

        switch (OuterSwitchOP) {
        case '1': {
            cout << "Enter filename(ex: img.ppm): ";
            getline(cin, f_name);

            if (read(f_name, img, w, h)) {
                Negative(img, w, h);
                string negativeIMG = "negative(" + f_name + ").ppm";
                write(negativeIMG, img, w, h);
                cout << "Negative image saved as " << negativeIMG;
                cout << endl;
            }
            free(img, h);
            break;
        }

        case '2': {
            cout << "Enter filename(ex: img.ppm): ";
            getline(cin, f_name);

            if (read(f_name, img, w, h)) {
                Gray(img, w, h);
                string grayIMG = "gray(" + f_name + ").ppm";
                write(grayIMG, img, w, h);
                cout << "Gray image saved as " << grayIMG;
                cout << endl;
            }
            free(img, h);
            break;
        }

        case '3': {
            string file1, file2;
            cout << "Enter 1st img_name(ex: img.ppm): ";
            getline(cin, file1);
            cout << "Enter 2nd img_name(ex: img.ppm): ";
            getline(cin, file2);

            if (read(file1, img, w, h) && read(file2, img2, w, h)) {
                img3 = allocate(h, w);

                float alpha;
                cout << "Enter alpha increment value (e.g., 0.1, 0.2, 0.05): ";
                cin >> alpha;

                while (alpha <= 0.0 || alpha > 1.0) {
                    cout << "Error: Enter B/W (0.0-1.0): ";
                    cin >> alpha;
                }

                float currentAlpha = 0.0;
                int counter = 0;
                while (currentAlpha <= 1.0) {
                    string morphIMG = "morph(" + to_string(counter) + ").ppm";
                    Combine(img, img2, img3, w, h, currentAlpha);
                    write(morphIMG, img3, w, h);
                    cout << "Saved morphing frame: " << morphIMG << endl;
                    currentAlpha += alpha;
                    counter++;

                }
            }
            free(img, h);
            free(img2, h);
            free(img3, h);
            break;
        }
        case '4':
        {
            string file1, file2;
            cout << "Enter 1st img_name(ex: img.ppm): ";
            getline(cin, file1);
            cout << "Enter 2nd img_name(ex: img.ppm): ";
            getline(cin, file2);

            if (read(file1, img, w, h) && read(file2, img2, w, h)) {
                img3 = allocate(h, w);

                float alpha;
                cout << "Enter alpha B/W (0.0-1.0): ";
                cin >> alpha;

                while (alpha <= 0.0 || alpha > 1.0) {
                    cout << "Error: Enter B/W (0.0-1.0): ";
                    cin >> alpha;
                }

                string combineIMG = "combine(" + file1 + "_" + file2 + ").ppm";
                Combine(img, img2, img3, w, h, alpha);
                write(combineIMG, img3, w, h);
                cout << "Combined image saved as " << combineIMG << endl;


                free(img3, h);
            }

            free(img, h);
            free(img2, h);
            break;
        }

        case '5':
        {
            string file1, file2;
            cout << "Enter 1st img_name(ex: img.ppm): ";
            getline(cin, file1);
            cout << "Enter 2nd img_name(ex: img.ppm): ";
            getline(cin, file2);

            if (read(file1, img, w, h) && read(file2, img2, w, h)) {
                img3 = allocate(h, w);

                Subtract(img, img2, img3, w, h);

                string subtractIMG = "subtract(" + file1 + "_" + file2 + ").ppm";
                write(subtractIMG, img3, w, h);
                cout << "Subtracted image saved as " << subtractIMG << endl;
            }
            free(img, h);
            free(img2, h);
            free(img3, h);
            break;
        }
        case '6':
        {
            
            while (done==false) {
                cout << "\nChoose an option:\n";
                cout << "1. Create Red Image\n2. Create Green Image\n3. Create Blue Image\n";
                cout << "Enter your choice: ";
                cin >> InnerSwitchOP;
                cin.ignore();

                switch (InnerSwitchOP) {
                case '1': {
                    cout << "Enter filename(ex: img.ppm): ";
                    getline(cin, f_name);
                    
                    if (read(f_name, img, w, h)) {
                        Red(img, w, h);
                        string redIMG = "red(" + f_name + ").ppm";
                        write(redIMG, img, w, h);
                        cout << "Red image saved as " << redIMG << endl;
                        done = true;
                    }
                    free(img, h);
                   
                    break;
                }

                case '2': {
                    cout << "Enter filename(ex: img.ppm): ";
                    getline(cin, f_name);

                    if (read(f_name, img, w, h)) {
                        Green(img, w, h);
                        string greenIMG = "green(" + f_name + ").ppm";
                        write(greenIMG, img, w, h);
                        cout << "Green image saved as " << greenIMG << endl;
                        done = true;
                    }
                    free(img, h);
                   
                    break;
                }

                case '3': {
                    cout << "Enter filename(ex: img.ppm): ";
                    getline(cin, f_name);

                    if (read(f_name, img, w, h)) {
                        Blue(img, w, h);
                        string blueIMG = "blue(" + f_name + ").ppm";
                        write(blueIMG, img, w, h);
                        cout << "Blue image saved as " << blueIMG << endl;
                        done = true;
                    }
                    free(img, h);
                    
                    break;
                }
                default:
                    cout << "Invalid choice." << endl;
                    break;
                }
            }
        case '7':
            cout << "Exiting program." << endl;
            return 0;

        default:
            cout << "Invalid choice." << endl;
            break;

            return 0;
        }
        }
    }
}
