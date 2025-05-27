#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <filesystem>

using namespace std;
namespace fs = std::filesystem;

// ANSI color codes
const string RED = "\033[31m";
const string GREEN = "\033[32m";
const string YELLOW = "\033[33m";
const string BLUE = "\033[34m";
const string MAGENTA = "\033[35m";
const string CYAN = "\033[36m";
const string RESET = "\033[0m";

// Unicode symbols for UI flair
const string CHECK_MARK = "\u2714";      // ✔
const string RIGHT_ARROW = "\u27A4";     // ➤
const string SPARKLES = "\u2728";        // ✨
const string STAR = "\u2B50";            // ⭐
const string WARNING = "\u26A0";         // ⚠

class Message {
    string sender;
    string content;
    time_t timestamp;

public:
    Message(const string& sender, const string& content)
        : sender(sender), content(content), timestamp(time(nullptr)) {}

    Message(const string& sender, const string& content, time_t ts)
        : sender(sender), content(content), timestamp(ts) {}

    string toString() const {
        stringstream ss;
        tm* timeinfo = localtime(&timestamp);
        ss << "[" << put_time(timeinfo, "%Y-%m-%d %H:%M:%S") << "] "
           << GREEN << sender << RESET << ": " << content;
        return ss.str();
    }
};

class ChatRoom {
    string roomName;
    vector<Message> messages;

public:
    ChatRoom() : roomName("default") {}
    ChatRoom(const string& name) : roomName(name) {}

    void addMessage(const Message& msg) {
        messages.push_back(msg);
    }

    void showHistory() const {
        cout << CYAN << STAR << "=== Chat History for Room: " << roomName << " ===" << RESET << "\n";
        for (const auto& msg : messages) {
            cout << RIGHT_ARROW << " " << msg.toString() << "\n";
        }
        cout << CYAN << "======================================\n" << RESET;
    }

    const string& getName() const { return roomName; }

    void saveMessageToFile(const Message& msg) const {
        ofstream out(roomName + "_history.txt", ios::app);
        if (!out) return;
        out << msg.toString() << "\n";
        out.close();
    }

    void loadFromFile() {
        messages.clear();
        ifstream in(roomName + "_history.txt");
        if (!in) return;

        string line;

        // Helper to strip ANSI color codes
        auto removeAnsi = [](const string& s) {
            string res;
            bool skip = false;
            for (char c : s) {
                if (c == '\033') { skip = true; continue; }
                if (skip) {
                    if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) skip = false;
                    continue;
                }
                res += c;
            }
            return res;
        };

        while (getline(in, line)) {
            // Expected format:
            // [YYYY-MM-DD HH:MM:SS] sender: content
            size_t firstBracket = line.find(']');
            if (firstBracket == string::npos) continue;
            string timeStr = line.substr(1, firstBracket - 1);
            string rest = line.substr(firstBracket + 2);

            size_t colonPos = rest.find(": ");
            if (colonPos == string::npos) continue;

            string senderColored = rest.substr(0, colonPos);
            string content = rest.substr(colonPos + 2);

            string sender = removeAnsi(senderColored);

            // Parse timestamp
            tm tm = {};
            istringstream ss(timeStr);
            ss >> get_time(&tm, "%Y-%m-%d %H:%M:%S");
            time_t timestamp = mktime(&tm);

            messages.emplace_back(sender, content, timestamp);
        }

        in.close();
    }
};

class ChatSimulator {
    map<string, ChatRoom> chatRooms;
    string currentRoom;
    string currentUser;

public:
    ChatSimulator(const string& username) : currentUser(username) {
        loadRoomsFromDisk();
    }

    void loadRoomsFromDisk() {
        for (const auto& entry : fs::directory_iterator(".")) {
            string filename = entry.path().filename().string();
            if (filename.size() > 12 && filename.substr(filename.size() - 12) == "_history.txt") {
                string roomName = filename.substr(0, filename.size() - 12);
                if (chatRooms.find(roomName) == chatRooms.end()) {
                    ChatRoom room(roomName);
                    room.loadFromFile();
                    chatRooms.emplace(roomName, room);
                }
            }
        }
    }

    void joinRoom(const string& roomName) {
        if (chatRooms.find(roomName) == chatRooms.end()) {
            chatRooms.emplace(roomName, ChatRoom(roomName));
            cout << GREEN << CHECK_MARK << " Created and joined ";
        } else {
            cout << GREEN << CHECK_MARK << " Joined ";
        }
        currentRoom = roomName;
        cout << "room: " << YELLOW << currentRoom << RESET << "\n";
    }

    void sendMessage(const string& content) {
        if (currentRoom.empty()) {
            cout << RED << WARNING << " Join a room first using /join <roomname>" << RESET << "\n";
            return;
        }
        Message msg(currentUser, content);
        chatRooms[currentRoom].addMessage(msg);
        chatRooms[currentRoom].saveMessageToFile(msg);
        cout << CYAN << RIGHT_ARROW << " Message sent." << RESET << "\n";
    }

    void showHistory() {
        if (currentRoom.empty()) {
            cout << RED << WARNING << " Join a room first." << RESET << "\n";
            return;
        }
        chatRooms[currentRoom].showHistory();
    }

    void listRooms() {
        if (chatRooms.empty()) {
            cout << YELLOW << STAR << " No chat rooms available yet." << RESET << "\n";
            return;
        }
        cout << CYAN << STAR << " Available chat rooms:" << RESET << "\n";
        for (const auto& [name, room] : chatRooms) {
            cout << " " << RIGHT_ARROW << " " << name << "\n";
        }
    }
};

int main() {
    cout << CYAN << "========================================" << RESET << "\n";
    cout << MAGENTA << "    💬 Welcome to Chat Simulator 💬    " << RESET << "\n";
    cout << YELLOW << "         " << SPARKLES << " Have a vibrant chat! " << SPARKLES << "         " << RESET << "\n";
    cout << CYAN << "========================================\n" << RESET;

    cout << BLUE << RIGHT_ARROW << " Enter your username: " << RESET;
    string username;
    getline(cin, username);

    ChatSimulator chat(username);

    cout << "\n" << MAGENTA << "Commands:" << RESET << "\n";
    cout << YELLOW << "  /join roomname  " << RESET << "- Join or create a chat room\n";
    cout << YELLOW << "  /history        " << RESET << "- Show chat history of current room\n";
    cout << YELLOW << "  /rooms          " << RESET << "- List all chat rooms\n";
    cout << YELLOW << "  /exit           " << RESET << "- Quit the program\n";
    cout << GREEN << "Type your messages to send to the current room.\n" << RESET;

    string input;
    while (true) {
        cout << MAGENTA << STAR << " " << RESET;
        getline(cin, input);

        if (input == "/exit") break;

        if (input.rfind("/join ", 0) == 0) {
            string roomName = input.substr(6);
            if (roomName.empty()) {
                cout << RED << WARNING << " Please specify a room name." << RESET << "\n";
                continue;
            }
            chat.joinRoom(roomName);
        } else if (input == "/history") {
            chat.showHistory();
        } else if (input == "/rooms") {
            chat.listRooms();
        } else if (!input.empty()) {
            chat.sendMessage(input);
        }
    }

    cout << CYAN << CHECK_MARK << " Goodbye!" << RESET << "\n";
    return 0;
}
