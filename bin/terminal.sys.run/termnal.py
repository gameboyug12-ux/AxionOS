import customtkinter as ctk
import requests
import ctypes
import paramiko
app = ctk.CTk()
app.geometry("500x500")
app.title("systemterminal")

# Entry
entry = ctk.CTkEntry(app, width=400, height=30)
entry.place(x=10, y=10)

# Output
textbox = ctk.CTkTextbox(app, width=480, height=420)
textbox.place(x=10, y=50)
textbox.configure(state="disabled")


def ask_url():
    dialog = ctk.CTkInputDialog(
        text="Unesi URL paketa:",
        title="Package Installer"
    )
    return dialog.get_input()


def install(url):
    try:
        r = requests.get(url)

        if r.status_code != 200:
            return f"Download failed ({r.status_code})"

        with open("/tmp/package.deb", "wb") as f:
            f.write(r.content)

        return "Package downloaded."

    except Exception as e:
        return str(e)


def execute(cmd):

    parts = cmd.split()

    if not parts:
        return "Empty command"

    action = parts[0]

    if action == "install":

        url = None

        for p in parts[1:]:
            if p.startswith("url="):
                url = p.split("=", 1)[1]

        # install url=
        if url == "":
            url = ask_url()

        if not url:
            return "Missing url="

        return install(url)

    elif action == "update":
        upadate()
        return "Updating..."
    
    elif action == "chdisk":
        if action == "/r":
            return ctypes.PyDLL("/lib/dashRchdisk.run")
        elif action == "/br":
            return ctypes.PyDLL("/lib/dashBrchdisk.run")

    return "Unknown command"
sysversion = "v1.0"
def update():
    ssh = paramiko.SSHClient
    ssh.set_missing_host_key_policy(paramiko.AutoAddPolicy())
    ssh.connect(
        hostname="178.220.185.236",
        username="update",
        password="123:3upadte"
    )
    stdin, stdout, stderr = ssh.exec_command(
        "cat /home/update/version.txt"
    )
    version = stdout.read().decode().strip()
    if version != sysversion:
        return update()
    else:
        return 0

def sendcode():

    cmd = entry.get()

    result = execute(cmd)

    textbox.configure(state="normal")
    textbox.insert("end", f"> {cmd}\n{result}\n\n")
    textbox.configure(state="disabled")

    entry.delete(0, "end")


# Dugme
button = ctk.CTkButton(app, text="Send", command=sendcode)
button.place(x=415, y=10)

# Enter taster
entry.bind("<Return>", lambda event: sendcode())

app.mainloop()