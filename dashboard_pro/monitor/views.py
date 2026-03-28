from django.shortcuts import render
import redis


UPSTASH_URL = "rediss://default:Put_Your_Password_here@prime-snake-66782.upstash.io:6379"
r = redis.Redis.from_url(UPSTASH_URL, decode_responses=True)

def live_dashboard(request):
    
    all_keys = r.keys('*')
    ip_data = []

    for key in all_keys:
        count = r.get(key)
        ip_data.append({'ip': key, 'count': count})

    # Yeh data hum apne aage banne wale HTML page ko bhejenge
    return render(request, 'dashboard.html', {'traffic_data': ip_data})