VERSION 5.00
Object = "{F9043C88-F6F2-101A-A3C9-08002B2F49FB}#1.2#0"; "comdlg32.ocx"
Begin VB.Form Form1 
   Caption         =   "Form1"
   ClientHeight    =   8655
   ClientLeft      =   60
   ClientTop       =   360
   ClientWidth     =   8745
   LinkTopic       =   "Form1"
   ScaleHeight     =   8655
   ScaleWidth      =   8745
   StartUpPosition =   3  'Windows Default
   WindowState     =   2  'Maximized
   Begin VB.PictureBox Picture1 
      BackColor       =   &H8000000E&
      Height          =   10575
      Left            =   0
      ScaleHeight     =   10515
      ScaleWidth      =   15195
      TabIndex        =   2
      Top             =   480
      Width           =   15255
      Begin VB.Label Label3 
         Alignment       =   2  'Center
         BackColor       =   &H8000000E&
         Caption         =   "(0,0)"
         Height          =   255
         Left            =   0
         TabIndex        =   5
         Top             =   0
         Width           =   975
      End
      Begin VB.Line Line4 
         BorderColor     =   &H8000000D&
         X1              =   360
         X2              =   840
         Y1              =   360
         Y2              =   360
      End
      Begin VB.Line Line3 
         BorderColor     =   &H8000000D&
         X1              =   14600
         X2              =   14600
         Y1              =   9840
         Y2              =   10080
      End
      Begin VB.Label Label2 
         Alignment       =   2  'Center
         BackColor       =   &H8000000E&
         Caption         =   "(0,0)"
         Height          =   375
         Left            =   14160
         TabIndex        =   4
         Top             =   10080
         Width           =   975
      End
      Begin VB.Label Label1 
         Alignment       =   2  'Center
         BackColor       =   &H8000000E&
         Caption         =   "(0,0)"
         Height          =   375
         Left            =   120
         TabIndex        =   3
         Top             =   10080
         Width           =   375
      End
      Begin VB.Line Line2 
         BorderColor     =   &H8000000D&
         X1              =   90
         X2              =   15240
         Y1              =   9960
         Y2              =   9960
      End
      Begin VB.Line Line1 
         BorderColor     =   &H8000000D&
         X1              =   600
         X2              =   600
         Y1              =   0
         Y2              =   10560
      End
   End
   Begin VB.CommandButton cmdDrawFloorplan 
      Caption         =   "Draw Floor Plan"
      Enabled         =   0   'False
      BeginProperty Font 
         Name            =   "Times New Roman"
         Size            =   11.25
         Charset         =   0
         Weight          =   700
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   375
      Left            =   2880
      TabIndex        =   1
      Top             =   0
      Width           =   2775
   End
   Begin MSComDlg.CommonDialog CommonDialog1 
      Left            =   8160
      Top             =   0
      _ExtentX        =   847
      _ExtentY        =   847
      _Version        =   393216
   End
   Begin VB.CommandButton Command1 
      Caption         =   "Select File "
      BeginProperty Font 
         Name            =   "Times New Roman"
         Size            =   11.25
         Charset         =   0
         Weight          =   700
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   375
      Left            =   0
      TabIndex        =   0
      Top             =   0
      Width           =   2775
   End
End
Attribute VB_Name = "Form1"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit
Dim fso As New FileSystemObject
Dim height1 As Integer, width1 As Integer
Dim scalex1 As Double, scaley1 As Double
Dim modulecount As Integer
Dim x1 As Integer, x2 As Integer
Dim y1 As Integer, y2 As Integer
Private Sub cmdDrawFloorplan_Click()
drawfloorplan
End Sub

Private Sub Command1_Click()
    CommonDialog1.FileName = ""
    CommonDialog1.Filter = "*.info"
    CommonDialog1.ShowOpen
    
   
    If CommonDialog1.FileName <> "" Then
    MsgBox "selected file name is" & CommonDialog1.FileName
    Else
    MsgBox "file not selected"
    Exit Sub
    
    End If
    
    If CommonDialog1.FileName <> "" Then
      cmdDrawFloorplan.Enabled = True
    End If

End Sub
Public Sub drawfloorplan()
     Picture1.Line (900, 0)-(900, 0)
     Picture1.Print CommonDialog1.FileName
     
        Dim ts1 As TextStream
        Dim j As Long
        
        
                
                Set ts1 = fso.OpenTextFile(CommonDialog1.FileName, ForReading)
                j = 1
                While ts1.AtEndOfStream = False
                    
                    Dim str As String
                    str = ts1.ReadLine
                    str = Trim$(str)
                    
                 MsgBox str
                 If Trim(str) = "" Then
                 Exit Sub
                 End If
                 
                 If j = 1 Then
                 
                 Dim first As Integer
                 first = InStr(str, " ")
                 modulecount = Trim(Mid(str, 1, first))
                 'MsgBox modulecount
                 
                 str = Trim(Mid(str, first, Len(str) - first + 1))
                 first = InStr(str, " ")
                 width1 = Trim(Mid(str, 1, first))
                 Label2.Caption = "(" & width1 & ",0)"
                 'MsgBox width1
                 scalex1 = 14000 / width1
                 
                 height1 = Trim(Mid(str, first, Len(str) - first + 1))
                 Label3.Caption = "(0," & height1 & ")"
                 'MsgBox height1
                 
                 scaley1 = 9600 / height1
                 
                 If scaley1 > scalex1 Then
                 scaley1 = scalex1
                 height1 = 9600 / scalex1
                Label3.Caption = "(0," & height1 & ")"
                 Else
                 scalex1 = scaley1
                 width1 = 14000 / scaley1
                 Label2.Caption = "(" & width1 & ",0)"
                 End If
                 
                 Else
                 
                 first = InStr(str, " ")
                 x1 = Trim(Mid(str, 1, first))
                 'MsgBox x1
                 
                 str = Trim(Mid(str, first, Len(str) - first + 1))
                 first = InStr(str, " ")
                 y1 = Trim(Mid(str, 1, first))
                 'MsgBox y1
                                  
                 str = Trim(Mid(str, first, Len(str) - first + 1))
                 first = InStr(str, " ")
                 x2 = Trim(Mid(str, 1, first))
                 'MsgBox x2
                 
                 y2 = Trim(Mid(str, first, Len(str) - first + 1))
                 'MsgBox y2
                 
                 setInImagecoordinet
                 'Picture1.DrawMode = True
                 Picture1.Line (x1, y1)-(x2, y1)
                 Picture1.Line (x1, y2)-(x2, y2)
                 Picture1.Line (x1, y1)-(x1, y2)
                 Picture1.Line (x2, y1)-(x2, y2)
                 Picture1.Line ((x1 + x2) / 2, (y1 + y2) / 2)-((x1 + x2) / 2, (y1 + y2) / 2)
                 Picture1.Print "(" & j - 1 & ")"
                 
                 End If
                    j = j + 1
               Wend
               MsgBox "totalmodule no" & j - 2
                If ts1.AtEndOfStream = True Then
                    MsgBox "File Read Succesfully"
                End If
                ts1.Close
                Set ts1 = Nothing
        
        End Sub
        
        Private Sub setInImagecoordinet()
            x1 = x1 * scalex1
            y1 = y1 * scaley1
            
            x1 = x1 + 600
            y1 = 9960 - y1
            
             x2 = x2 * scalex1
            y2 = y2 * scaley1
            
            x2 = x2 + 600
            y2 = 9960 - y2
        End Sub
