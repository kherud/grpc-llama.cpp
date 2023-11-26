#include <iostream>
#include <memory>
#include <string>
#include <grpcpp/grpcpp.h>

#include "llm.grpc.pb.h"

using namespace de::kherud::grpc::llm;

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

class LLMClient {
  public:
    LLMClient(std::shared_ptr<Channel> channel)
        : stub_(LLM::NewStub(channel)) {}

    // Assembles the client's payload, sends it and presents the response back
    // from the server.
    void status() {
        // Data we are sending to the server.
        GetStatusRequest request;

        // Container for the data we expect from the server.
        GetStatusReply reply;

        // Context for the client. It could be used to convey extra information to
        // the server and/or tweak certain RPC behaviors.
        ClientContext context;

        // The actual RPC.
        Status status = stub_->status(&context, request, &reply);

        // Act upon its status.
        if (status.ok()) {
        	std::cout << "available" << std::endl;
            std::cout << reply.available() << std::endl;
        } else {
        	std::cout << "not available" << std::endl;
            std::cout << status.error_code() << ": " << status.error_message() << std::endl;
        }
    }

    private:
        std::unique_ptr<LLM::Stub> stub_;
};

int main(int argc, char** argv) {
    LLMClient client(
        grpc::CreateChannel("0.0.0.0:50051", grpc::InsecureChannelCredentials())
    );
    client.status();

    return 0;
}
