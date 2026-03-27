from fastapi import FastAPI, Request
from fastapi.responses import JSONResponse
import redis

app = FastAPI()

# APNI URL YAHAN DAAL (Dhyan rakhna rediss:// se start ho)
UPSTASH_URL = "rediss://default:PUT_Your_Passrowrd_here@prime-snake-66782.upstash.io:6379"

print("[*] Redis se connect karne ki koshish kar rahe hain...")
try:
    # decode_responses=True matlab raw bytes nahi, normal string aayegi
    r = redis.Redis.from_url(UPSTASH_URL, decode_responses=True)
    r.ping() # Yeh database ko ek 'Hi' bhejta hai check karne ke liye
    print("[+] SUCCESS: Upstash Redis ekdum mast connect ho gaya!")
except Exception as e:
    print(f"[-] ERROR: Redis connect nahi hua! Issue: {e}")

@app.middleware("http")
async def rate_limit_middleware(request: Request, call_next):
    client_ip = request.client.host
    print(f"\n[1] Request aayi from IP: {client_ip}")
    
    try:
        print("[2] Redis check kar raha hu...")
        requests_made = r.get(client_ip)
        print(f"[3] Redis ne current count bataya: {requests_made}")
        
        if requests_made is None:
            r.setex(client_ip, 60, 1)
        elif int(requests_made) >= 5:
            print(f"[BLOCKED] IP: {client_ip} ne spam kiya!")
            return JSONResponse(
                status_code=429, 
                content={"error": "Do not spam, Limit for 1 min is 5 requests"}
            )
        else:
            r.incr(client_ip)

        print("[4] Request allowed. Sending to Backend")
        response = await call_next(request)
        return response

    except Exception as e:
        print(f"[-] MIDDLEWARE ERROR: {e}")
        return JSONResponse(status_code=500, content={"error": "Internal Server Error"})

@app.get("/")
def read_root():
    return {"message": "Your Request has passed C++, and reached Backend!"}