# include <fstream>
# include <iostream>
# include <string>
# include <unordered_map>
# include <queue>
# include <stdint.h>

const std::unordered_map<std::string, uint8_t> opcode{
    {"nop",   0x00}, {"exit", 0x01}, {"bp",   0x02}, 
    {"store", 0x04}, {"load", 0x05}, {"push", 0x06}, {"pop",   0x07},
    {"copy",  0x08}, {"sout", 0x0a}, {"casl", 0x0b}, 
    {"addl",  0x0c}, {"subl", 0x0d}, {"andl", 0x0e}, {"orl",   0x0f},
    {"addr",  0x10}, {"subr", 0x11}, {"mulr", 0x12}, {"divr",  0x13},
    {"addi",  0x14}, {"subi", 0x15}, {"muli", 0x16}, {"divi",  0x17},
    {"gt",    0x18}, {"ge",   0x19}, {"eq",   0x1a}, {"const", 0x1b},
    {"and",   0x1c}, {"or",   0x1d}, {"not",  0x1e}, 
    {"goto",  0x20}, {"back", 0x21},
    {"ifgt",  0x24}, {"ifge", 0x25}, {"ifeq", 0x26}, {"iout",  0x27},
    {"iin",   0x28}, {"new",  0x29}, {"set",  0x2a}, {"get",   0x2b},
    {"call",  0x2c}, {"ret",  0x2d},
    {"shlr",  0x30}, {"shrr", 0x31},
    {"shli",  0x34}, {"shri", 0x35}
}, reg{
    {"r0",  0x00}, {"r1",  0x01}, {"r2",  0x02}, {"r3",  0x03},
    {"r4",  0x04}, {"r5",  0x05}, {"r6",  0x06}, {"r7",  0x07},
    {"r8",  0x08}, {"r9",  0x09}, {"r10", 0x0a}, {"r11", 0x0b},
    {"r12", 0x0c}, {"r13", 0x0d}, {"r14", 0x0e}, {"r15", 0x0f}
};

void synerr(std::string errmsg) {
    std::cerr << errmsg << std::endl;
    return;
}

bool if_num(std::string token) {
    for (uint32_t i = 0; i < token.length(); i++) if (token[i] < '0' || '9' < token[i]) return false;
    return true;
}

uint8_t token_identification(std::string token) { //tokenを受け取ってtoken_type(uint8_t型)を返す
    if (opcode.find(token) != end(opcode)) return 'o';
    else if (reg.find(token) != end(reg))  return 'r';
    else if (if_num(token)) return 'n';
    else return 'l';
}

int32_t main(){
    std::string path = "test.txt";

    uint8_t* objcode = new uint8_t[1024];
    uint8_t bufadr = 0;
    int32_t line = 0;
    std::string str;
    std::unordered_map<std::string, uint8_t> label;
    std::queue<std::pair<std::string, uint8_t>> label_replace;

    std::ifstream ifs(path);
    if (ifs.fail()) synerr("Error: Can't read file.");

    while (std::getline(ifs, str)) {
        
        line++;
        std::cout << "line:" << line << std::endl;
        
        int strlen = str.length();
        
        if (str[0] == '\r' || str[0] == '\n') continue;
        else if (strlen == 1 && (str[0] == '\t' || str[0] == ' ')) continue; // EOF処理
        else if (str[0] == '\t') {
            int offset = 1;
            int token_num = 0;
            while(offset < strlen) {
                if (str[offset] == '\r' || str[offset] == '\n' || offset == str.size() - 1 ) { // 最後のはEOFのとき
                    // tokenが0個だったらbufadr+=4しない
                    if (token_num == 0) continue;
                    else if ( token_num < 5) {
                        for (uint32_t j = token_num; j < 5; j++) {
                            objcode[bufadr + j] = 0;
                        }
                    break;
                    }
                }
                else if ('!' <= str[offset] && str[offset] <= '~') {
                    int head = offset;
                    while ('!' <= str[offset] && str[offset] <= '~' && offset != str.size() - 1) {
                        offset++;
                    }
                    std::string token = str.substr(head, offset - head);
                    switch (token_identification(token)) {
                        case 'o':
                            if (token_num != 0) synerr("Error: Invalid token sequence.");
                            objcode[bufadr + token_num] = opcode.at(token);
                            break;
                        case 'r':
                            if (token_num == 0) synerr("Error: Invalid token sequence.");
                            objcode[bufadr + token_num] = reg.at(token);
                            break;
                        case 'n':
                            if (token_num == 0) synerr("Error: Invalid token sequence.");
                            objcode[bufadr + token_num] = uint8_t(std::stoi(token));
                            break;
                        case 'l':
                            if (token_num == 0) synerr("Error: Invalid token sequence."); //todo: ラベルが重なってた時にerrにしたい
                            label_replace.emplace(token, bufadr + token_num);
                            break;
                    }
                    token_num++;
                }
                else if (str[offset] == ' ' || str[offset] == '\t') offset++;
                else synerr("Error: Invalid letter.");
            }
            bufadr += 4;
        }
        else if ('!' <= str[0] && str[0] <= '~') {
            // todo?:subのlenが0だったら?
            for (uint32_t j = 0; j < strlen; j++) {
                if (str[j] == ':') {
                    label.emplace(str.substr(0, j), bufadr);
                    break;
                }
                if (j == strlen - 1) synerr("Error: Need ':'.");
            }
        }
        else synerr("Error: Invalid letter.");
    }
    
    while(!label_replace.empty()) {
        std::pair<std::string, uint8_t> p = label_replace.front();
        label_replace.pop();
        if (label.find(p.first) == end(label)) synerr("Error: Invalid label.");
        else objcode[p.second] = label[p.first];
    }
    
    for (uint32_t i = 0; i < bufadr; i++) {
        std::cout << static_cast<uint32_t>(objcode[i]);
        // 4つごとに改行
        if (i % 4 == 3) std::cout << std::endl;
        else std::cout << "\t";
    }

    return 0;
}

