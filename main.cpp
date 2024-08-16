#include <fstream>
#include <ios>
#include <iostream>
#include <vector>


struct Header {
    uint8_t creationCentury = 0;
    uint8_t creationYear = 0;
    uint8_t creationMonth = 0;
    uint8_t creationDay = 0;
    uint32_t numberOfGames = 0;
    uint16_t numberOfPlayers = 0;
    uint16_t gameYear = 0;
    uint8_t boardSize = 0;
    uint8_t isSolitaire = 0;
    uint8_t theoreticalEstimationDepth = 0;
    uint8_t reservedByte = 0;
};

enum GameType {
    kUnknown,
    k8by8,
    k10by10,
    kSolitaire
};

Header ParseHeader(std::ifstream& is) {
    Header header;
    if (is.read(reinterpret_cast<char*>(&header.creationCentury), 1)) {
        std::cout << "creationCentury: " << +header.creationCentury << std::endl;
    }
    if (is.read(reinterpret_cast<char*>(&header.creationYear), 1)) {
        std::cout << "creationYear: " << +header.creationYear << std::endl;
    }
    if (is.read(reinterpret_cast<char*>(&header.creationMonth), 1)) {
        std::cout << "creationMonth: " << +header.creationMonth << std::endl;
    }
    if (is.read(reinterpret_cast<char*>(&header.creationDay), 1)) {
        std::cout << "creationDay: " << +header.creationDay << std::endl;
    }
    if (is.read(reinterpret_cast<char*>(&header.numberOfGames), 4)) {
        std::cout << "numberOfGames: " << header.numberOfGames << std::endl;
    }
    if (is.read(reinterpret_cast<char*>(&header.numberOfPlayers), 2)) {
        std::cout << "numberOfPlayers: " << header.numberOfPlayers << std::endl;
    }
    if (is.read(reinterpret_cast<char*>(&header.gameYear), 2)) {
        std::cout << "gameYear: " << header.gameYear << std::endl;
    }
    if (is.read(reinterpret_cast<char*>(&header.boardSize), 1)) {
        std::cout << "boardSize: " << +header.boardSize << std::endl;
    }
    if (is.read(reinterpret_cast<char*>(&header.isSolitaire), 1)) {
        std::cout << "isSolitaire: " << +header.isSolitaire << std::endl;
    }
    if (is.read(reinterpret_cast<char*>(&header.theoreticalEstimationDepth), 1)) {
        std::cout << "theoreticalEstimationDepth: " << +header.theoreticalEstimationDepth << std::endl;
    }
    if (is.read(reinterpret_cast<char*>(&header.reservedByte), 1)) {
        std::cout << "reservedByte: " << +header.reservedByte << std::endl;
    }
    return header;
}

void ParseAndPrintGames(std::ifstream& is, const Header& header, GameType gameType) {
    for (int i = 0; i < header.numberOfGames; ++i) {
        std::cout << "========================================================================================" << std::endl;
        uint16_t tournamentNumber = 0;
        if (is.read(reinterpret_cast<char*>(&tournamentNumber), 2)) {
            std::cout << "tournamentNumber: " << tournamentNumber << std::endl;
        }
        uint16_t blackPlayerNumber = 0;
        if (is.read(reinterpret_cast<char*>(&blackPlayerNumber), 2)) {
            std::cout << "blackPlayerNumber: " << blackPlayerNumber << std::endl;
        }
        uint16_t whitePlayerNumber = 0;
        if (is.read(reinterpret_cast<char*>(&whitePlayerNumber), 2)) {
            std::cout << "whitePlayerNumber: " << whitePlayerNumber << std::endl;
        }
        uint8_t blackDiscsActual = 0;
        if (is.read(reinterpret_cast<char*>(&blackDiscsActual), 1)) {
            std::cout << "blackDiscsActual: " << +blackDiscsActual << std::endl;
        }
        uint8_t blackDiscsTheoretical = 0;
        if (is.read(reinterpret_cast<char*>(&blackDiscsTheoretical), 1)) {
            std::cout << "blackDiscsTheoretical: " << +blackDiscsTheoretical << std::endl;
        }
        const int moveCount = gameType == GameType::k8by8 ? 60 : 96;
        // The rowColCoeff helps to extract row and column from the integer encoding the move.
        const int rowColCoeff = gameType == GameType::k8by8 ? 10 : 12;
        std::vector<uint8_t> listOfMoves(moveCount, 0);
        if (is.read(reinterpret_cast<char*>(listOfMoves.data()), moveCount)) {
            std::cout << "listOfMoves:" << std::endl;
            bool firstIter = true;
            for (int m: listOfMoves) {
                if (firstIter) {
                    firstIter = false;
                } else {
                    std::cout << ", ";
                }
                int row = m / rowColCoeff;
                int col = m % rowColCoeff - 1;
                std::cout << static_cast<char>(col + 'a') << row;
            }
            std::cout << std::endl << std::endl;
        }
    }
}

int main() {
    const std::string filepath = "data/WTH_2021.wtb";
    std::ifstream is(filepath, std::ios_base::binary);
    if (!is) {
        throw std::runtime_error("failed to read file: " + filepath);
    }
    const Header header = ParseHeader(is);
    GameType gameType = GameType::kUnknown;
    if (header.isSolitaire) {
        gameType = GameType::kSolitaire;
    } else {
        if (header.boardSize == 0 || header.boardSize == 8) {
            gameType = GameType::k8by8;
        } else if (header.boardSize == 10) {
            gameType = GameType::k10by10;
        }
    }
    if (gameType == GameType::kSolitaire) {
        throw std::runtime_error("Error! File contains solitaire games. Solitaire mode is not supported yet!");
    }
    ParseAndPrintGames(is, header, gameType);
    return 0;
}
