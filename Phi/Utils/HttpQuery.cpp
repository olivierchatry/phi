#include <PreCompile.h>
#include <Utils/HttpQuery.h>

namespace Utils
{
    HttpQuery::HttpQuery()
    {
        mHttpStatus = 0;
        mConnectionStatus = Free;

    }

    void HttpQuery::pump()
    {
        while( mConnection->outstanding() )
            mConnection->pump();
    }
    
    bool HttpQuery::query(const std::string& server, int port, const std::string& url)
    {
        if (mConnectionStatus != Free)
            return false;
        mConnectionStatus = Receiving;
        mConnection.reset(new happyhttp::Connection(server.c_str(), port));
        mConnection->setcallbacks(HttpQuery::onBegin, HttpQuery::onData, HttpQuery::onComplete, this);
        mConnection->request("GET", url.c_str());
        mThread = std::thread(&HttpQuery::pump, this);
        return true;
    }
    

    void HttpQuery::onBegin( const happyhttp::Response* r, void* userdata )
    {
        HttpQuery* query = (HttpQuery*) userdata;
        query->mHttpStatus = r->getstatus();
    }
    
    void HttpQuery::onData( const happyhttp::Response* r, void* userdata, const unsigned char* data, int n )
    {
        HttpQuery* query = (HttpQuery*) userdata;
        std::string temp;
        temp.resize(n);
        memcpy(&temp[0], data, n);
        query->mResult.insert(query->mResult.end(), temp.begin(), temp.end());
    }
    
    void HttpQuery::onComplete( const happyhttp::Response* r, void* userdata )
    {
        HttpQuery* query = (HttpQuery*) userdata;
        query->mConnectionStatus = Free;
    }
    
};