#pragma once

namespace Utils
{
    struct HttpQuery
    {
    public:
        enum
        {
            Free,
            Receiving
        };
        
    public:
        HttpQuery();
        
    public:
        bool            query(const std::string& server, int port, const std::string& url);

    public:
        std::string&    result()    { return mResult; }
        int             httpStatus()    { return mHttpStatus; }
        int             connectionStatus() { return mConnectionStatus; }
    public:
        void                    pump();

    public:
        static void onBegin( const happyhttp::Response* r, void* userdata );
        static void onData( const happyhttp::Response* r, void* userdata, const unsigned char* data, int n );
        static void onComplete( const happyhttp::Response* r, void* userdata );


    private:
        int                     mHttpStatus;
        std::string             mResult;
        std::thread             mThread;
        std::shared_ptr<happyhttp::Connection>  mConnection;
        int                     mConnectionStatus;
    };
}