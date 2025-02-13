import fs
from processor import convert_text_to_db
from tkinter import Menu, messagebox
import tkinter as tk

from view.table import Table
from view.workspace import Workspace


class App(tk.Tk):
    def __init__(self):
        super().__init__()
        self.workspace = Workspace(self)
        self.table = Table(self)

        self.title("LW1")
        self.geometry('1024x640')
        self.resizable(True, True)
        self.create_widgets()
        self.mainloop()

    def create_widgets(self):
        main_menu = Menu(self)

        file_menu = Menu(main_menu, tearoff=0)
        file_menu.add_command(label="Open", command=lambda: fs.open_file(lambda content: self.workspace.set_text(content)))
        file_menu.add_separator()
        file_menu.add_command(label="Exit", command=self.quit)

        main_menu.add_cascade(label="File", menu=file_menu)

        about_menu = Menu(main_menu, tearoff=0)
        about_menu.add_command(label="About", command=self.show_about)
        main_menu.add_cascade(label="Help", menu=about_menu)

        self.config(menu=main_menu)
        self.center_window(1024, 640)
        self.__init_event_handlers()

    def __init_event_handlers(self):
        self.workspace.on_text_changed.append(lambda text: self.table.set_data(convert_text_to_db(text)))

    def center_window(self, w, h):
        ws, hs = self.winfo_screenwidth(), self.winfo_screenheight()
        x, y = (ws / 2) - (w / 2), (hs / 2) - (h / 2)

        self.geometry('%dx%d+%d+%d' % (w, h, x, y))

    def show_about(self):
        messagebox.showinfo("About", "праграма для языков чтоб разложить текст")
