#include <iostream>
#include <string>
#include <random>
#include <algorithm>


#include <curl/curl.h>
#include <jsoncpp/json/json.h>


std::string generateRandomCards(int amount)
{
    std::vector<std::string> selectedCards;
    std::vector<std::string> cards = {
        "The Fool (0)", 
        "The Magician (1)", 
        "The High Priestess (2)", 
        "The Empress (3)", 
        "The Emperor (4)", 
        "The Hierophant (5)", 
        "The Lovers (6)", 
        "The Chariot (7)", 
        "Strength (8)", 
        "The Hermit (9)", 
        "Wheel of Fortune (10)", 
        "Justice (11)", 
        "The Hanged Man (12)", 
        "Death (13)", 
        "Temperance (14)", 
        "The Devil (15)", 
        "The Tower (16)", 
        "The Star (17)", 
        "The Moon (18)", 
        "The Sun (19)", 
        "Judgment (20)", 
        "The World (21)"
    };
    if (amount <= 0 || amount > cards.size()) {
        std::cerr << "Invalid amount requested." << std::endl;
        return "";
    }


    std::random_device rd;
    std::mt19937 gen(rd());
    std::shuffle(cards.begin(), cards.end(), gen);


    // Randomly select cards
    for (int i = 0; i < amount; ++i) {
        selectedCards.push_back(cards[i]);
    }
    std::string representation = "";
    for (int i = 0; i < selectedCards.size(); ++i) {
        representation += selectedCards[i] + ((i+1) == amount ? "" : ", ");
    }

    return representation;

}

// Callback function to handle response data
size_t write_callback(void *contents, size_t size, size_t nmemb, std::string *output) {
    output->append((char*)contents, size * nmemb);
    return size * nmemb;
}

std::string generate_text(const std::string& prompt, const std::string& model_id) {
    CURL *curl;
    CURLcode res;
    std::string answer = "";
    std::string readBuffer;

    curl = curl_easy_init();
    if (curl) {
        std::string url = "https://api-inference.huggingface.co/models/" + model_id;
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_POST, 1L);

        // Set request headers
        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        headers = curl_slist_append(headers, "Authorization: Bearer <Token>");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);

        // Construct JSON payload
        
        Json::Value root;
        root["inputs"] = Json::Value(prompt);
        root["parameters"] = Json::Value();
        root["parameters"]["max_new_tokens"] = 2000; 
        Json::StreamWriterBuilder writer;
        std::string json_data = Json::writeString(writer, root);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_data.c_str());
        
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        
        res = curl_easy_perform(curl);
        /* Check for errors */
        if (res != CURLE_OK) {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        }

        //Json::Value json_bufer(readBuffer);
        //answer = json_bufer["generated_text"];

        /* Clean up */
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    }
    return readBuffer;
}

int main()
{
    
    // Specify the prompt and model ID
    std::string question;
    std::cout << "Enter a question: ";
    std::getline(std::cin, question);

    std::string cards = generateRandomCards(5);
    std::string prompt = "You're a tarot card fortune teller with a lot of experience.A client wants to ask you a question. You have to answer the question based solely on the cards you've been dealt. You use 5 Card Tarot Spread. They ask you: " + question + "The cards are: " + cards +". Make a prediction based on the cards you've been dealt. Role play only yourself.\\n\\nTarot card teller: ";
    
    std::string model_id = "mistralai/Mistral-7B-Instruct-v0.2";

    // Call the function to generate text based on the prompt
    std::string generated_text = generate_text(prompt, model_id);


    // Check if the response is not empty
    if (generated_text.empty()) {
        // Print an error message if the response is empty
        std::cerr << "Failed to generate text." << std::endl;
    }
    generated_text = generated_text.substr(prompt.size()+3);
    generated_text = generated_text.substr(0, generated_text.size()-3);
    
    // Print out the generated text
    std::cout << cards << std::endl;
    std::cout << "Generated Text:" << std::endl;
    std::cout << generated_text << std::endl;
    
    return 0;
}