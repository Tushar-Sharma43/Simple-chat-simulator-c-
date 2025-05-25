#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <fstream>

using namespace std;

// ANSI Color Codes
const string RED = "\033[31m";
const string GREEN = "\033[32m";
const string YELLOW = "\033[33m";
const string BLUE = "\033[34m";
const string MAGENTA = "\033[35m";
const string CYAN = "\033[36m";
const string RESET = "\033[0m";

// Emojis & Symbols
const string CHECK = "✔️";
const string PLUS = "➕";
const string EXIT = "🚪";
const string HISTORY = "📜";
const string PROMPT = "💬";
const string STAR = "⭐";
const string ALERT = "⚠️";
const string WAVE = "👋";
const string ROOM = "🏠";

class Message {
    string sender;
    string content;
    time_t timestamp;

public:
    Message(const string& sender, const string& content)
        : sender(sender), content(content), timestamp(time(nullptr)) {}

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
    ChatRoom() = default;
    ChatRoom(const string& name) : roomName(name) {}

    void addMessage(const Message& msg) {
        messages.push_back(msg);
    }

    void showHistory() const {
        cout << BLUE << HISTORY << " Chat History for '" << roomName << "':" << RESET << "\n";
        for (const auto& msg : messages) {
            cout << PROMPT << " " << msg.toString() << "\n";
        }
        cout << CYAN << "----------------------------------" << RESET << "\n";
    }

    void saveToFile() const {
        ofstream file(roomName + "_history.txt", ios::app);
        for (const auto& msg : messages) {
            file << msg.toString() << "\n";
        }
        file.close();
    }

    const string& getName() const {
        return roomName;
    }
};

class ChatSimulator {
    map<string, ChatRoom> chatRooms;
    string currentRoom;
    string currentUser;

public:
    ChatSimulator(const string& user) : currentUser(user) {}

    void joinRoom(const string& roomName) {
        if (chatRooms.find(roomName) == chatRooms.end()) {
            chatRooms.emplace(roomName, ChatRoom(roomName));
            cout << GREEN << PLUS << " Created new room: " << ROOM << " " << roomName << RESET << "\n";
        } else {
            cout << GREEN << CHECK << " Joined existing room: " << ROOM << " " << roomName << RESET << "\n";
        }
        currentRoom = roomName;
    }

    void sendMessage(const string& msg) {
        if (currentRoom.empty()) {
            cout << RED << ALERT << " You must join a room first using /join <room>" << RESET << "\n";
            return;
        }
        Message message(currentUser, msg);
        chatRooms[currentRoom].addMessage(message);
        chatRooms[currentRoom].saveToFile();
        cout << GREEN << PROMPT << " Message sent to " << ROOM << " " << currentRoom << RESET << "\n";
    }

    void showHistory() const {
        if (currentRoom.empty()) {
            cout << RED << ALERT << " No room joined yet!" << RESET << "\n";
            return;
        }
        chatRooms.at(currentRoom).showHistory();
    }

    void listRooms() const {
        if (chatRooms.empty()) {
            cout << YELLOW << ALERT << " No rooms available yet." << RESET << "\n";
            return;
        }
        cout << MAGENTA << ROOM << " Available Rooms:\n" << RESET;
        for (const auto& [name, room] : chatRooms) {
            cout << "  - " << ROOM << " " << name << "\n";
        }
    }
};

int main() {
    cout << CYAN << "====================================" << RESET << "\n";
    cout << MAGENTA << "     💬 Welcome to ChatSim v1.0    " << RESET << "\n";
    cout << CYAN << "====================================" << RESET << "\n";

    cout << BLUE << "Enter your username " << WAVE << ": " << RESET;
    string user;
    getline(cin, user);

    ChatSimulator sim(user);

    cout << "\n" << STAR << " Commands:\n";
    cout << YELLOW << "  /join <room>   " << RESET << "- Join or create a chat room\n";
    cout << YELLOW << "  /history       " << RESET << "- Show chat history of current room\n";
    cout << YELLOW << "  /rooms         " << RESET << "- List all available chat rooms\n";
    cout << YELLOW << "  /exit          " << RESET << "- Exit the chat simulator\n";
    cout << MAGENTA << "------------------------------------\n" << RESET;

    string input;
    while (true) {
        cout << GREEN << PROMPT << " > " << RESET;
        getline(cin, input);

        if (input == "/exit") {
            cout << BLUE << WAVE << " Goodbye!" << RESET << "\n";
            break;
        } else if (input.rfind("/join ", 0) == 0) {
            string roomName = input.substr(6);
            sim.joinRoom(roomName);
        } else if (input == "/history") {
            sim.showHistory();
        } else if (input == "/rooms") {
            sim.listRooms();
        } else if (!input.empty()) {
            sim.sendMessage(input);
        }
    }

    return 0;
}
