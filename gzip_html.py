Import("env")
import gzip, os, time

SRC = os.path.join("src", "index.html")
DST = os.path.join("data", "index.html.gz")

def gzip_html(source, target, env):
    if not os.path.exists(SRC):
        print("No index.html found to gzip.")
        return

    # Only re-gzip if source is newer
    if os.path.exists(DST) and os.path.getmtime(DST) >= os.path.getmtime(SRC):
        return

    with open(SRC, "rb") as f_in, gzip.open(DST, "wb", compresslevel=9) as f_out:
        f_out.writelines(f_in)

    print("Gzipped {} -> {}".format(SRC, DST))

# Hook into filesystem image build (SPIFFS/LittleFS)
env.AddPreAction("buildfs", gzip_html)