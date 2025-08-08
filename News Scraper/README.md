Build and start container with
```
$ sudo docker build -t news_scraper .
# interactive for testing
$ sudo docker run -it --rm -p 8000:8000 --name=news_scraper news_scraper
# detached with restart policy for deplyoment
$ sudo docker run -d --restart=unless-stopped -p 8000:8000 --name=news_scraper news_scraper
```
