﻿namespace Monosand;

public class Game
{
    public const int DefaultWindowWidth = 1280;
    public const int DefaultWindowHeight = 720;

    private static Game? instance;
    public static Game Instance
    {
        get => instance ?? throw SR.GameNotNewed;
        set => instance = instance is null ? value : throw SR.GameHasBeenNewed;
    }
    internal static Platform Platform => Instance.platform;

    private Window? mainWindow;
    internal Platform platform;

    public Window? MainWindow
    {
        get => mainWindow;
        set
        {
            if (value is null)
                throw new ArgumentNullException(nameof(value));
            mainWindow = value;
            mainWindow.Game = this;
        }
    }

    public Game(Platform platform, Window? mainWindow = null)
    {
        Instance = this;
        this.platform = platform;
        platform.Init();
        MainWindow = mainWindow;
    }

    public void Run()
    {
        MainWindow ??= new Window(this);
        MainWindow.Show();
        while (true)
        {
            MainWindow.PollEvents();
            if (MainWindow.IsInvalid) break;

            MainWindow.Update();
            MainWindow.RenderInternal();
            Thread.Sleep(10);
        }
    }
}