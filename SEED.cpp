#include <iostream>
#include <string>
#include <cstdint>
#include <random>

struct Seed
{
    uint32_t K0, K1, K2, K3, R0, R1, R2, R3, R4, R5;
};

Seed generateRandomSeed()
{

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 100);

    Seed seed;
    seed.K0 = dis(gen);
    seed.K1 = dis(gen);
    seed.K2 = dis(gen);
    seed.K3 = dis(gen);
    // Здесь можно использовать любой генератор псевдослучайных чисел
    // для генерации случайных значений seed.K0, seed.K1, seed.K2, seed.K3

    return seed;
}

void seedEncipher(uint32_t* plaintext, uint32_t* ciphertext, Seed* seed)
{
    uint32_t L0, L1, R0, R1, T0, T1;

    L0 = plaintext[0];
    L1 = plaintext[1];

    R0 = plaintext[2];
    R1 = plaintext[3];

    uint32_t K0 = seed->K0, K1 = seed->K1, K2 = seed->K2, K3 = seed->K3;
    uint32_t R2 = seed->R2, R3 = seed->R3, R4 = seed->R4, R5 = seed->R5;

    for (int i = 0; i < 16; i++)
    {
        T0 = R0 ^ K0;
        T1 = R1 ^ K1;
        T1 ^= T0;

        T1 = ((T1 << 1) | (T1 >> 31)) & 0xFFFFFFFF;
        T1 += T0;
        T1 &= 0xFFFFFFFF;

        T0 = ((T0 << 5) | (T0 >> 27)) & 0xFFFFFFFF;
        T0 += T1;
        T0 &= 0xFFFFFFFF;

        T1 = ((T1 << 7) | (T1 >> 25)) & 0xFFFFFFFF;
        T1 ^= T0;

        T0 = ((T0 << 13) | (T0 >> 19)) & 0xFFFFFFFF;
        T0 = (T0 << 3) | (T0 >> 29);

        T1 ^= T0;

        T0 = ((T0 << 5) | (T0 >> 27)) & 0xFFFFFFFF;
        T1 -= T0;
        T1 &= 0xFFFFFFFF;

        T1 = (T1 << 1) | (T1 >> 31);

        T0 = ((T0 << 7) | (T0 >> 25)) & 0xFFFFFFFF;
        T1 -= T0;
        T1 &= 0xFFFFFFFF;

        T0 = ((T0 << 13) | (T0 >> 19)) & 0xFFFFFFFF;

        R0 = R2 ^ T0;
        R1 = R3 ^ T1;

        R2 = R0;
        R3 = R1;

        R0 = L0;
        R1 = L1;

        L0 = R2;
        L1 = R3;
    }

    ciphertext[0] = L0;
    ciphertext[1] = L1;
    ciphertext[2] = R0;
    ciphertext[3] = R1;
}

std::string stringToHex(const std::string& str)
{
    std::string result;
    for (char c : str)
    {
        result += "0123456789ABCDEF"[((c >> 4) & 0xF)];
        result += "0123456789ABCDEF"[((c & 0xF))];
    }
    return result;
}

std::string hexToString(const std::string& hexStr)
{
    std::string result;
    for (size_t i = 0; i < hexStr.length(); i += 2)
    {
        char c = (hexStr[i] >= 'A' ? (hexStr[i] - 'A' + 10) : (hexStr[i] - '0')) << 4;
        c |= (hexStr[i + 1] >= 'A' ? (hexStr[i + 1] - 'A' + 10) : (hexStr[i + 1] - '0'));
        result += c;
    }
    return result;
}

int main()
{
    std::string plaintext;
    std::cout << "Введите текст для шифрования: ";
    std::getline(std::cin, plaintext);

    uint32_t plaintextLength = plaintext.length();

    // Проверка на необходимую длину текста для шифрования
    while (plaintextLength % 16 != 0)
    {
        std::cout << "Длина текста должна быть кратной 16!" << std::endl;
        std::cout << "Введите текст для шифрования: ";
        std::getline(std::cin, plaintext);
        uint32_t plaintextLength = plaintext.length();
        //return 0;
    
   

    // Преобразование текста в массив int32_t
    uint32_t* plaintextArray = new uint32_t[plaintextLength / 4];
    for (int i = 0; i < plaintextLength / 4; i++)
    {
        plaintextArray[i] = (plaintext[i * 4] << 24) |
            (plaintext[i * 4 + 1] << 16) |
            (plaintext[i * 4 + 2] << 8) |
            (plaintext[i * 4 + 3]);
    }

    // Генерация сида
    Seed seed = generateRandomSeed();

    // Подготовка массива для хранения зашифрованного текста
    uint32_t* ciphertextArray = new uint32_t[plaintextLength / 4];

    // Шифрование каждого блока текста
    for (int i = 0; i < plaintextLength / 16; i++)
    {
        seedEncipher(plaintextArray + i * 4, ciphertextArray + i * 4, &seed);
    }

    // Вывод зашифрованного текста в шестнадцатеричном формате
    std::cout << "Зашифрованный текст: " << stringToHex(std::string(reinterpret_cast<char*>(ciphertextArray), plaintextLength)) << std::endl;

    // Дешифрование каждого блока текста
    for (int i = 0; i < plaintextLength / 16; i++)
    {
        seedEncipher(ciphertextArray + i * 4, plaintextArray + i * 4, &seed);
    }

    // Преобразование массива int32_t в исходную строку
    std::string decryptedText(reinterpret_cast<char*>(plaintextArray), plaintextLength);
    std::cout << "Расшифрованный текст: " << decryptedText << std::endl;

    delete[] plaintextArray;
    delete[] ciphertextArray;

    return 0;
}
}
